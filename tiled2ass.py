#!/usr/bin/python3
import sys, os
import argparse
import struct
import json
import gzip
import zlib
from base64 import b64decode
from pathlib import Path

parser = argparse.ArgumentParser(description='Convert a Tiled JSON map to A3X map data.')
parser.add_argument('inFile', help='source file')
parser.add_argument('outFile', nargs='?', help='target .s or .c file')
args = parser.parse_args()

stem = Path(args.inFile).stem
if not args.outFile:
	args.outFile = stem + '.s'
asS = not Path(args.outFile).suffix == '.c'

inf = open(args.inFile, "r")
mapJSON = json.load(inf)
inf.close()

data = {}
palettes = {}

tileset = mapJSON['tilesets'][0]
if 'tiles' in tileset:
	for tile in tileset['tiles']:
		tID = int(tile['id'])
		for property in tile['properties']:
			if property['name'] == 'palette':
				palettes[tID] = int(property['value'])

for layer in mapJSON['layers']:
	if layer['type'] != 'tilelayer':
		continue;
	if not layer['visible']:
		continue;
	mapName = layer['name'].replace(' ', '')
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
	tileOffset = 0
	if 'properties' in layer:
		for property in layer['properties']:
			if property['name'] == 'tileOffset':
				tileOffset = int(property['value'])
	newData = []
	for value in mapData:
		v = ((value - 1) & 0x3FF) if (value > 0) else 0
		o = v
		v += tileOffset
		if (value & 0x80000000) == 0x80000000:
			v |= 0x0400 # horizontal flip
		if (value & 0x40000000) == 0x40000000:
			v |= 0x0800 # vertical flip
		if o in palettes:
			v |= (palettes[o] << 12)
		newData.append(v)
	data[mapName] = newData

of = open(args.outFile, "w")

if asS:
	of.write('\t.section .rodata\n')
	of.write('\t.align 2\n')
	for key in data:
		of.write(f'\t.global {key}\n')

for key,item in data.items():
	if asS:
		of.write(f'{key}:')
	else:
		of.write(f'const uint16_t {key}[{len(item)}] = {{')
	i = 0
	for v in item:
		if i % 8 == 0:
			of.write('\n\t.short ' if asS else '\n\t')
		elif asS:
			of.write(',')
		i += 1
		of.write(f'0x{v:04X}')
		if not asS:
			of.write(', ')
	of.write('\n')
	if not asS:
		of.write('};\n')
