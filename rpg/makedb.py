#!/usr/bin/python3
import sys, os, io
import argparse
import struct
import json
from pathlib import Path

parser = argparse.ArgumentParser(description='Convert an A3X RPG database from JSON to source code.')
parser.add_argument('inFile', default='database.json', nargs='?', help='source file')
parser.add_argument('outFile', nargs='?', help='target .s file')
args = parser.parse_args()

stem = Path(args.inFile).stem
if not args.outFile:
	args.outFile = stem + '.s'

inf = open(args.inFile, "r")
dbJS = json.load(inf)
inf.close()

of = open(args.outFile, "w")

of.write('\t.global party, opponentDB, formationsDB\n')
of.write('\t.text\n')
of.write('\n');

of.write('party:\n')
for partymember in dbJS['partymembers']:
	bits = 0
	hp = partymember['hp']
	pp = partymember['pp']
	name = partymember['name']
	of.write(f'//{name}\n')
	of.write(f'\t.long 0x{bits:08X}\n') # consider using defines
	of.write(f'\t.short {hp}, {hp}\n')
	of.write(f'\t.short {pp}, {pp}\n')
	of.write(f'\t.asciz "{name}"; .skip {12-len(name)}\n')
	of.write(f'\t.byte 0\n') # TODO
name = '\\0' * 14
if len(dbJS['partymembers']) < 8:
	skips = 8 - len(dbJS['partymembers'])
	of.write(f'//filler\n')
	of.write(f'\t.skip {skips * 26}\n')
	
of.write('\n')

opponents = []
of.write('opponentDB:\n')
for opponent in dbJS['opponents']:
	bits = 0
	hp = opponent['hp']
	pp = opponent['pp']
	name = opponent['name']
	handler = opponent['handler']
	opponents.append(name.upper());
	of.write(f'//{name}\n')
	of.write(f'\t.long 0x{bits:08X}\n') # consider using defines
	of.write(f'\t.short {hp}, {pp}\n')
	of.write(f'\t.asciz "{name}"; .skip {15-len(name)}\n')
	if handler == None:
		handler = '0'
	of.write(f'\t.long {handler}\n')
of.write('\n')
i = 1
for opponent in opponents:
	of.write(f'#define m{opponent} {i}\n')
	i = i + 1
of.write('\n')

of.write('formationsDB:\n')
for formation in dbJS['formations']:
	of.write('\t.byte ')
	for i in range(0, len(formation)):
		target = 0
		if i < len(formation):
			target = 'm' + formation[i].upper()
		of.write(f'{target}')
		if i < len(formation) - 1:
			of.write(', ')
		else:
			of.write(f'; .skip {6 - len(formation)}\n')

of.write('\n')
