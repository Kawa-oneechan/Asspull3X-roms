export TARGET :=	navigator
export SFILES :=	crt0.s
export CFILES :=	main.c ui.c view.c icons.c ../lab/std.c ../ass-rt.c
include ../ass_romrules

icons.c : icons.png
	$(GRIT) icons.png -ftc -fh! -p! -th16 -gB1 -gu8
