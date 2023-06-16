#!/usr/bin/python3
import struct, sys, os, io
import argparse
from PIL import Image
from pathlib import Path

parser = argparse.ArgumentParser(description='Convert an image to the A3X .api format.')
parser.add_argument('inFile', help='source image file')
parser.add_argument('outFile', nargs='?', help='target .api file')
parser.add_argument('-r', '--raw', help='skip compression', action='store_true')
parser.add_argument('-v', '--verbose', help='use verbose output', action='store_true')
parser.add_argument('-g', '--nograds', help='skip HDMA gradients', action='store_true')
parser.add_argument('-c', '--clipgrads', help='clip HDMA gradients', action='store_true')
parser.add_argument('-t', '--truepal', help='store minimal palette data', action='store_true')
args = parser.parse_args()

stem = Path(args.inFile).stem
if not args.outFile:
	args.outFile = stem + '.api'
	if args.verbose:
		print(f'No output file given, assuming {args.outFile}.')

im = Image.open(args.inFile)
if im.mode != 'P':
	print('Image is not indexed.')
	quit()

inPal = im.getpalette()
outData = bytes(im.getdata())

truePalLength = max(outData) + 1
fourBits = truePalLength < 17
if args.verbose:
	if fourBits:
		print('Image uses only 16 colors.')
	else:
		print('Image uses more than 16 colors.');

stride = im.width
if fourBits:
	stride = im.width // 2

palSize = 32 if fourBits else 512
if args.truepal:
	palSize = truePalLength * 2
palLength = 16 if fourBits else 256
palOffset = 0x18
dataOffset = palOffset + palSize

size = im.width * im.height
if not fourBits:
	size /= 2

compressed = True
depth = 4 if fourBits else 8

if fourBits:
	newData = bytearray()
	i = 0
	while i < len(outData) - 1:
		theFour = outData[i + 0]
		theFour |= outData[i + 1] << 4
		i += 2
		newData.append(theFour)
	outData = newData

def rleCompress(data):
	if args.verbose:
		print(f'Attempting to compress {len(data)} bytes...')
	ret = bytearray()
	i, count = 0, 0

	def emit(data, i, count):
		if i >= len(data):
			return
		if data[i] >= 0xC0 or count > 0:
			ret.append(0xC0 | (count + 1))
		ret.append(data[i])
		return 0

	while i < len(data) - 1:
		if data[i] == data[i + 1]:
			if count == 62:
				count = emit(data, i, count)
			else:
				count += 1
		else:
			count = emit(data, i, count)
		i += 1
	count = emit(data, i, count)
	ret.append(0xC0)
	ret.append(0xC0)
	if args.verbose:
		print(f'Ended up with {len(ret)} bytes.')
	return ret

if args.raw:
	compressed = False
	if args.verbose:
		print('Skipping compression by request.')
else:
	compData = rleCompress(outData)
	if len(compData) < len(outData):
		outData = compData
	else:
		compressed = False

dataSize = len(outData)
dataOffset = palOffset + palSize

flags = 1 if compressed else 0

hdmaChannels = []
for i in range(0, 8):
	hdmaFile = stem + f'-h{i}.png'
	if os.path.exists(hdmaFile):
		hdmaChannels.append(i)
if args.nograds:
	if len(hdmaChannels):
		print(f'Ignoring {len(hdmaChannels)} HDMA channel(s).')
	hdmaChannels = []
if len(hdmaChannels) > 0:
	flags = flags | 2
	palOffset = palOffset + 4
	dataOffset = dataOffset + 4
hdmaOffset = dataOffset + dataSize

if args.verbose:
	print(f'depth: {depth}')
	print(f'flags: {flags}')
	print(f'width: {im.width}, height: {im.height}, stride: {stride}')
	print(f'palLength: {palLength} (true {truePalLength})')
	print(f'palSize: 0x{palSize:X} ({palSize})')
	print(f'palOffset: 0x{palOffset:X}')
	print(f'dataSize: 0x{len(outData):X}')
	print(f'dataOffset: 0x{dataOffset:X}')
	if len(hdmaChannels) > 0:
		print(f'hdma: {hdmaChannels}')
		print(f'hdmaOffset: 0x{hdmaOffset:X}')

bf = io.BytesIO()
bf.write(b'AIMG')
bf.write(struct.pack('>b', depth))
bf.write(struct.pack('>b', flags))
bf.write(struct.pack('>H', im.width))
bf.write(struct.pack('>H', im.height))
bf.write(struct.pack('>H', stride))
bf.write(struct.pack('>L', im.height * stride)) #len(outData)))
bf.write(struct.pack('>L', palOffset))
bf.write(struct.pack('>L', dataOffset))
if len(hdmaChannels) > 0:
	bf.write(struct.pack('>L', hdmaOffset))	

for i in range(palLength):
	if i < truePalLength:
		r, g, b = inPal[(i * 3) + 0], inPal[(i * 3) + 1], inPal[(i * 3) + 2]
	else:
		if args.truepal:
			break;
		r, g, b = 0, 0, 0
	snes = ((b >> 3) << 10) | ((g >> 3) << 5) | (r >> 3)
	bf.write(struct.pack('>H', snes))

bf.write(outData)

if len(hdmaChannels) > 0:
	bf.write(struct.pack('>b', len(hdmaChannels)))
	for i in hdmaChannels:
		clipgrads = args.clipgrads
		im = Image.open(stem + f'-h{i}.png')
		if im.mode != 'RGB':
			im = im.convert('RGB')
		tl = im.getpixel((0,0))
		tr = im.getpixel((im.size[0]-1,0))
		if tr == (255, 255, 255) and tl != (255, 255, 255):
			clipgrads = False
			if args.verbose:
				print(f'Disabling clipping for channel {i}.')
		imgh = im.size[1]
		gradient = []
		for y in range(imgh):
			r, g, b = im.getpixel((0, y))
			snes = ((b >> 3) << 10) | ((g >> 3) << 5) | (r >> 3)
			gradient.append(snes)
		start = 0
		stop = imgh
		if clipgrads:
			first = gradient[start]
			last = gradient[stop - 1]
			for y in range(start, stop):
				if gradient[y] != first:
					start = y
					break
			for y in range(stop - 1, start + 2, -1):
				if gradient[y] != last:
					stop = y
					break
			gradient = gradient[start:stop]
		if imgh <= 240:
			start = start * 2
			stop = stop * 2
		length = stop - start
		hdmaControl = 1 | (1 << 4) | (start << 8) | (length << 20)
		if imgh == 240:
			hdmaControl = hdmaControl | 0x80
		bf.write(struct.pack('>H', len(gradient) * 2))
		bf.write(struct.pack('>L', hdmaControl))
		for y in gradient:
			bf.write(struct.pack('>H', y))
		if args.verbose:
			print(f'hdma[{i}]: 0x{hdmaControl:0>8X}, size {len(gradient)}, {start} to {stop}, for {length}')

if Path(args.outFile).suffix == '.c' or Path(args.outFile).suffix == '.s':
	asS = not Path(args.outFile).suffix == '.c'
	of = open(args.outFile, "w")
	data = bf.getvalue()

	if asS:
		of.write('\t.section .rodata\n')
		of.write('\t.align 2\n')
		of.write(f'\t.global {stem}\n')
	
	if asS:
		of.write(f'{stem}:')
	else:
		of.write(f'const unsigned char {stem}[{len(data)}] = {{')
	i = 0
	for v in data:
		if i % 16 == 0:
			of.write('\n\t.byte ' if asS else '\n\t')
		elif asS:
			of.write(',')
		i += 1
		of.write(f'0x{v:02X}')
		if not asS:
			of.write(', ')
	of.write('\n')
	if not asS:
		of.write('};\n')
else:
	with open(args.outFile, "wb") as of:
		of.write(bf.getbuffer())
