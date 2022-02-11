#!/usr/bin/python3
import sys, os
import argparse
from PIL import Image
from pathlib import Path

parser = argparse.ArgumentParser(description='Convert a color strip to HMDA color data.')
parser.add_argument('inFile', help='source picture')
parser.add_argument('outFile', nargs='?', help='target .s or .c file')
parser.add_argument('identifier', nargs='?', help='identifier name')
parser.add_argument('-a', '--append', help='append to target file', action='store_true')
args = parser.parse_args()

stem = Path(args.inFile).stem
if not args.outFile:
	args.outFile = stem + '.s'
if not args.identifier:
	args.identifier = stem

im = Image.open(args.inFile)
if im.mode != 'RGB':
	im = im.convert('RGB')
imgh = im.size[1]

asS = not Path(args.outFile).suffix == '.c'

of = open(args.outFile, "w" if not args.append else "a")

if asS:
	of.write('\t.section .rodata\n' if not args.append else '\n')
	of.write('\t.align 2\n')
	of.write(f'\t.global {args.identifier}\n')
	of.write(f'{args.identifier}:')
else:
	of.write(f'const uint16_t {args.identifier}[{imgh}] = {{')

for y in range(imgh):
	r, g, b = im.getpixel((0, y))
	snes = ((b >> 3) << 10) | ((g >> 3) << 5) | (r >> 3)
	if y % 8 == 0:
		of.write('\n\t.short ' if asS else '\n\t')
	elif asS:
		of.write(',')
	of.write(f'0x{snes:04X}')
	if not asS:
		of.write(', ')
of.write('\n')
if not asS:
	of.write('};\n')
