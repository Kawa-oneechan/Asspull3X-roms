GRITS := $(wildcard *.grit)
GRAPHICS := $(GRITS:.grit=.png)
GFXAPI := title.png diskbg.png

export TARGET :=	sokoban
export SFILES :=	crt0.s hdma.s assets.s
export CFILES :=	main.c ../imfplay.c $(GFXAPI:.png=.c) $(GRAPHICS:.png=.c)
include ../ass_romrules

