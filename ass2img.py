#!/usr/bin/python3
import struct, sys, os
import argparse
from PIL import Image
from pathlib import Path

parser = argparse.ArgumentParser(description='Display an A3X .api file or convert it to a regular image.')
parser.add_argument('inFile', help='.api file to display or save')
parser.add_argument('outFile', nargs='?', help='if specified, file to save as')
parser.add_argument('-v', '--verbose', help='use verbose output', action='store_true')
args = parser.parse_args()

imf = open(args.inFile, 'rb')
magic, = struct.unpack('4s', imf.read(4))
if magic != b'AIMG':
	print('Input file is not a valid Asspull IIIx image.')
	quit()

# TODO: more checks?
depth, compressed = struct.unpack('BB', imf.read(2))
width, height, stride = struct.unpack('>HHH', imf.read(3 * 2))
palSize = 256 if depth == 8 else 16
dataSize, palOffset, dataOffset = struct.unpack('>LLL', imf.read(3 * 4))

if args.verbose:
	print(f'depth: {depth}, compressed: {compressed}')
	print(f'width: {width}, height: {height}, stride: {stride}')
	print(f'palSize: {palSize}')
	print(f'palOffset: {palOffset}')
	print(f'dataSize: {dataSize}')
	print(f'dataOffset: {dataOffset}')

imf.seek(palOffset)
palData = imf.read(palSize * 2)

imf.seek(dataOffset)
fileSize = Path(args.inFile).stat().st_size

screen = bytearray()
palette = bytearray()
for i in range(palSize):
	snes = palData[(i * 2) + 1] | (palData[(i * 2) + 0] << 8)
	r = (snes >> 0) & 0x1F
	g = (snes >> 5) & 0x1F
	b = (snes >> 10) & 0x1F
	palette.append((r << 3) + (r >> 2))
	palette.append((g << 3) + (r >> 2))
	palette.append((b << 3) + (r >> 2))

if compressed:
	while imf.tell() < fileSize:
		data, = struct.unpack('B', imf.read(1))
		if data & 0xC0 == 0xC0:
			rle = data & 0x3F
			original = data
			data, = struct.unpack('B', imf.read(1))
			if data == 0xC0 and rle == 0:
				break
			while rle > 0:
				if depth == 8:
					screen.append(data)
				else:
					screen.append((data >> 0) & 0x0F)
					screen.append((data >> 4) & 0x0F)
				rle -= 1
		else:
			if depth == 8:
				screen.append(data)
			else:
				screen.append((data >> 0) & 0x0F)
				screen.append((data >> 4) & 0x0F)
else:
	for i in range(dataSize):
		data, = struct.unpack('B', imf.read(1))
		if depth == 8:
			screen.append(data)
		else:
			screen.append((data >> 0) & 0x0F)
			screen.append((data >> 4) & 0x0F)

img = Image.new('P', (width, height))
img.putpalette(palette)
img.putdata(screen)		
if args.outFile:
	img.save(args.outFile)
else:
	img.show()
