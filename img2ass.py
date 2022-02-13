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

fourBits = max(outData) < 17
if args.verbose:
	if fourBits:
		print('Image uses only 16 colors.')
	else:
		print('Image uses more than 16 colors.');

stride = im.width
if fourBits:
	stride = im.width // 2

palSize = 32 if fourBits else 512
palLength = 16 if fourBits else 256

size = im.width * im.height
if not fourBits:
	size /= 2

compressed = True

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

if args.verbose:
	print(f'depth: {4 if fourBits else 8}, compressed: {1 if compressed else 0}')
	print(f'width: {im.width}, height: {im.height}, stride: {stride}')
	print(f'palSize: {palLength}')
	print(f'palOffset: {0x18}')
	print(f'dataSize: {len(outData)}')
	print(f'dataOffset: {0x18 + palSize}')

bf = io.BytesIO()
bf.write(b'AIMG')
bf.write(b'\x04' if fourBits else b'\x08')
bf.write(b'\x01' if compressed else b'\x00')
bf.write(struct.pack('>H', im.width))
bf.write(struct.pack('>H', im.height))
bf.write(struct.pack('>H', stride))
bf.write(struct.pack('>L', len(outData)))
bf.write(struct.pack('>L', 0x18))
bf.write(struct.pack('>L', 0x18 + palSize))

for i in range(palLength):
	r, g, b = inPal[(i * 3) + 0], inPal[(i * 3) + 1], inPal[(i * 3) + 2]
	snes = ((b >> 3) << 10) | ((g >> 3) << 5) | (r >> 3)
	bf.write(struct.pack('>H', snes))

bf.write(outData)

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
