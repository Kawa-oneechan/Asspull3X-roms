#!/usr/bin/python3
import sys, os, io
import argparse
import struct
import json
import gzip
import zlib
import sexpdata
from base64 import b64decode
from pathlib import Path

parser = argparse.ArgumentParser(description='Convert a Tiled JSON map and its entity script code to A3X RPG map data.')
parser.add_argument('inFile', help='source file')
parser.add_argument('outFile', nargs='?', help='target .s file')
args = parser.parse_args()

stem = Path(args.inFile).stem
if not args.outFile:
	args.outFile = stem + '.s'

inf = open(args.inFile, "r")
mapJSON = json.load(inf)
inf.close()

def compile(script):
	sexp = sexpdata.loads(script)
	mem = io.BytesIO()
	commands = {
		'nop': 0x00,
		'+': 0x01,
		'-': 0x02,
		'==': 0x05,
		'=': 0x06,
		
		'!push.v': 0x0C,
		'!push.b': 0x0D,
		'!push.w': 0x0E,
		'!push.s': 0x0F,
		'!push0': 0x10,
		'!push1': 0x11,
		'!push2': 0x12,
		'!dup': 0x13,
		'!store.v': 0x14,
		'!eq': 0x1A,
		'!jump': 0x20,
		'!jumpnot': 0x22,

		# Calls
		'say': 0x81,
		'face': 0x82,
		'ask': 0x83,
		'portrait': 0x84,
		'getpartyname': 0x85,

		# Constants
		'#x': 1,
		'#y': 2,
		'#spr': 3,

		# Variables
		'%playername': 2,

		# Meta
		'repeat': 0xFF,
		'while': 0xFF,
		'if': 0xFF,
	}
	for i in range(0, 255):
		commands['%' + str(i)] = i
	stringPool = {}

	#INPUT:  (print "narf!")
	#OUTPUT: 0F06001106FF6E6172662100
	
	def push(val, last):
		# if val == last:
		#	mem.write(struct.pack('B', 0x13)) #dup
		if val >= 0 and val <= 2:
			mem.write(struct.pack('B', 0x10 + val))
		elif val >= 0 and val <= 255:
			mem.write(struct.pack('BB', 0x0D, val)) #pushByte ..
		else:
			mem.write(struct.pack('>Bh', 0x0E, val)) #pushWord ....
		return val
	
	def process(arg, lastPush):
		# print (type(arg))
		## TODO: ID and support nested lists
		if isinstance(arg, sexpdata.Symbol):
			sym = arg.value()
			if not sym in commands:
				raise ValueError(f'Unknown symbol "{sym}"')
			if sym.startswith('%'):
				mem.write(struct.pack('BB', 0x0C, commands[sym])) #pushVar ..
			else:
				lastPush = push(commands[sym], lastPush)
		elif isinstance(arg, int):
			lastPush = push(arg, lastPush)
		elif isinstance(arg, str):
			offset = mem.tell() + 1
			mem.write(struct.pack('>BH', 0x0F, offset)) #pushPtr ....
			stringPool[offset] = arg
		elif isinstance(arg, list):
			eval(arg)
		return lastPush
	
	def eval(cons):
		lastPush = 0
		
		if isinstance(cons[0], sexpdata.Symbol):
			command = cons[0].value()
			if not command in commands:
				raise ValueError(f'Unknown command "{command}"')
			args = []
			for i in range(1, len(cons)):
				args.append(cons[i])
			args.reverse()
			comNo = commands[command]
			noArgs = (comNo == 0 or comNo == 7 or (comNo >= 0x1A and comNo <= 0x1F))
			
			if command == '+' and all([isinstance(item, int) for item in cons[1:]]):
				print(f'+ and all of {cons[1:]} are ints.')
				sum = 0
				for item in cons[1:]:
					sum += item
				print(f'+ result is {sum}')
				lastPush = push(sum, lastPush)
			elif command == '-' and all([isinstance(item, int) for item in cons[1:]]):
				print(f'- and all of {cons[1:]} are ints.')
				sum = cons[1]
				for item in cons[2:]:
					sum -= item
				print(f'- result is {sum}')
				lastPush = push(sum, lastPush)
			elif command == '=':
				target = cons[1]
				if isinstance(target, sexpdata.Symbol) and target.value().startswith('%'):
					# print(f'Heads up: storing into variable {target.value()}. As we ought.')
					lastPush = process(cons[2], lastPush)
					mem.write(struct.pack('Bb', 0x14, commands[target.value()])) #storeVar ..
				else:
					raise ValueError(f'Expected (= %var ...) but got (= {target.value} ...).')
			elif command == 'repeat':
				# (repeat (nop) (nop) (nop))
				# 0000	00	nop		<.
				# 0001	00	nop		 |
				# 0002	00	nop		 |
				# 0003	20	jump	 |
				# 0004	  FD	-5	-'
				# 0005	......
				repeatBack = mem.tell()
				for item in cons[1:]:
					eval(item)
				mem.write(struct.pack('Bb', 0x20, -(mem.tell() - repeatBack))) #jump ..
				### TODO
			elif command == 'if':
				# (if (== 4 4) (nop) (nop) (nop))
				# 0000	14	pushlit4
				# 0001	14	pushlit4
				# 0002	1A	eq?
				# 0003	22	jumpnot
				# 0004	  03	+3	-.
				# 0005	00	nop		 |
				# 0006	00	nop		 |
				# 0007	00	nop		 |
				# 0008	......		<'
				eval(cons[1])
				mem.write(struct.pack('B', 0x22));
				ifPoint = mem.tell()
				mem.write(struct.pack('B', 0)); # to fill in later
				for item in cons[2:]:
					eval(item)
				ifEnd = mem.tell()
				mem.seek(ifPoint)
				mem.write(struct.pack('b', ifEnd - ifPoint - 1))
				mem.seek(ifEnd)
			elif command == 'while':
				# (while (== 4 4) (nop) (nop) (nop))
				# 0000	14	pushlit4<.
				# 0001	14	pushlit4 |
				# 0002	1A	eq?		 |
				# 0003	22	jumpnot	 |-.
				# 0004	  05	+5	 | |
				# 0005	00	nop		 | |
				# 0006	00	nop		 | |
				# 0007	00	nop		 | |
				# 0008	20	jump	 | |
				# 0009	  F6	-10	-' |
				# 000A	......		  <'
				whileBack = mem.tell()
				eval(cons[1])
				mem.write(struct.pack('B', 0x22));
				whilePoint = mem.tell()
				mem.write(struct.pack('B', 0)); # to fill in later
				for item in cons[2:]:
					eval(item)
				mem.write(struct.pack('Bb', 0x20, -(mem.tell() - whileBack - 1)))
				whileEnd = mem.tell()
				mem.seek(whilePoint)
				mem.write(struct.pack('b', whileEnd - whilePoint - 2))
				mem.seek(whileEnd)				
			else:
				for arg in args:
					lastPush = process(arg, lastPush)
				if not noArgs:
					lastPush = push(len(args), lastPush)
				mem.write(struct.pack('B', commands[command]))
		else:
			print(f'What even is {cons}?')
	
	for car in sexp:
		eval(car)
	mem.write(struct.pack('B', 0xFF))
	
	theGoods = mem.getbuffer().tobytes()

	code = io.StringIO()
	
	i = 0
	while i < len(theGoods):
		b = theGoods[i]
		i += 1
		if b == 0xFF:
			code.write('\t.byte 0xFF //end of script\n')
			break
		
		c = False
		for k,v in commands.items():
			if v == b:
				c = k
				break
		if c == False:
			continue
		if c.startswith('!'): c = c[1:]
		code.write(f'\t.byte 0x{b:02X} //{c}\n')
		if b in [ 0x0C, 0x0D, 0x14, 0x20, 0x22 ]:
			b = theGoods[i]
			i += 1
			code.write(f'\t  .byte 0x{b:02X} //{b}\n')
		elif b in [ 0x0E ]:
			w = theGoods[i] << 8
			i += 1
			w |= theGoods[i]
			i += 1
			code.write(f'\t  .short 0x{w:04X} //{w}\n')
		elif b in [ 0x0F ]:
			w = theGoods[i] << 8
			i += 1
			w |= theGoods[i]
			i += 1
			code.write(f'\t  .long STRPOOL_{w}\n')

	for k,v in stringPool.items():
		code.write(f'STRPOOL_{k}:\n')
		code.write(f'\t.asciz "{v.encode("unicode_escape").decode("utf-8")}"\n')

	return code.getvalue()

# print(compile('((repeat (nop) (nop) (nop)))'))
# print(compile('((if (== 4 4) (nop) (nop) (nop)))'))
# print(compile('((while (== 4 4) (nop) (nop) (nop)))'))
# print(compile('((+ 1 2 3))'))
# print(compile('((- 3 1))'))
# print(compile('((= %42 69) (say "42 is %d." %42))'))
# print (compile('((say \"What a year, huh %s?\" (getpartyname 0) 1))'))

tileMap = []
mapName = 'testMap'
mapWidth = mapJSON['width']
mapHeight = mapJSON['height']
spriteLayer = 0
scripts = ''

for layer in mapJSON['layers']:
	if layer['type'] == 'objectgroup':
		spriteLayer = layer['objects']
		continue;
	if layer['type'] != 'tilelayer':
		continue;
	if not layer['visible']:
		continue;
	mapData = layer['data']
	if 'encoding' in layer:
		if layer['encoding'] == 'base64':
			mapData = b64decode(mapData)
			if layer['compression'] == 'gzip':
				mapData = gzip.decompress(mapData)
			elif layer['compression'] == 'zlib':
				mapData = zlib.decompress(mapData)
			fmt = '<%dL' % (len(mapData) // 4)
			mapData = list(struct.unpack(fmt, mapData))
	tileMap = []
	for value in mapData:
		v = ((value - 1) & 0x3FF) if (value > 0) else 0
		tileMap.append(v)

of = open(args.outFile, "w")

of.write('\t.text\n')
of.write(f'\t.global {mapName}\n')
of.write('\n');

of.write(f'{mapName}:\n')
of.write(f'\t.byte {mapWidth}, {mapHeight} //size\n')
of.write(f'\t.long __{mapName}_Map\n')
of.write(f'\t.long officeTiles\n') ### TODO
of.write(f'\t.long __{mapName}_Entities\n')
of.write('\n');

of.write(f'__{mapName}_Map:')
i = 0
for v in tileMap:
	if i % mapWidth == 0:
		of.write('\n\t.byte ')
	else:
		of.write(',')
	i += 1
	of.write(f'{v:3}')
of.write('\n')
of.write('\n')

of.write(f'__{mapName}_Entities:\n')
of.write(f'\t.byte {len(spriteLayer)}\n')
i = 0
for v in spriteLayer:
	of.write(f'//{i}\n')
	p = {}
	for prop in v['properties']:
		p[prop['name']] = prop['value']
	x = v['x'] // 16
	y = v['y'] // 16
	s = p['sprite']
	f = p['facing']
	m = p['motor']
	script = p['script']
	of.write(f'\t.byte {s}\n')
	of.write(f'\t.byte {x}, {y}\n')
	of.write(f'\t.byte {f}\n')
	of.write(f'\t.byte {m}\n')
	of.write(f'\t.byte 0\n')
	of.write(f'\t.long 0\n' if script == '' else f'\t.long __{mapName}_Entity{i}_Script\n')
	if script != '':
		scriptText = compile(script)
		scriptText = scriptText.replace('STRPOOL_', f'__{mapName}_Entity{i}_')
		scripts += '\n'
		scripts += f'__{mapName}_Entity{i}_Script:\n';
		scripts += scriptText
	i += 1

of.write(scripts)
of.write('\n')
