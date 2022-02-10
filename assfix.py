#!/usr/bin/python3
import struct, sys, os
import argparse
from pathlib import Path

parser = argparse.ArgumentParser(description='Pad an A3X .ap3 ROM file and fix its checksum.')
parser.add_argument('inFile', help='source .ap3 file')
parser.add_argument('outFile', nargs='?', help='target .ap3 file')
parser.add_argument('-v', '--verbose', help='use verbose output', action='store_true')
args = parser.parse_args()

if not args.outFile:
	args.outFile = args.inFile

fileSize = Path(args.inFile).stat().st_size

def roundup(v):
	v -= 1;
	v |= v >> 1
	v |= v >> 2
	v |= v >> 4
	v |= v >> 8
	v |= v >> 16
	v += 1
	return v

padded = roundup(fileSize)

with open(args.inFile, "rb") as inp:
	wft = inp.read()

sum = 0
for i in range(0, 0x20):
	sum += wft[i]
for i in range(0x20, fileSize):
	sum += wft[i]

if args.verbose:
	print(f'Checksum is {sum:08X}.')
	print(f'Padding from {fileSize:08X} ({fileSize // 1024} KiB) up to {padded:08X} ({padded // 1024} KiB).')

with open(args.outFile, "wb") as outp:
	outp.write(wft)
	outp.seek(0x20)
	outp.write(struct.pack('>L', sum))
	if padded > fileSize:
		outp.seek(padded - 1)
		outp.write(b'\x00')
