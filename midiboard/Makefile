GRITS := $(wildcard *.grit)
GRAPHICS := $(GRITS:.grit=.png)
GFXAPI := piano.png

export TARGET :=	midiboard
export SFILES :=	crt0.s
export CFILES :=	main.c $(GFXAPI:.png=.c) $(GRAPHICS:.png=.c)
include ../ass_romrules
