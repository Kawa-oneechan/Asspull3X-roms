GRITS := $(wildcard *.grit)
GRAPHICS := $(GRITS:.grit=.png)
TILED := girl1map.json

export TARGET :=	columns
export SFILES :=	crt0.s $(TILED:.json=.s)
export CFILES :=	main.c $(GRAPHICS:.png=.c) ../ass-rt.c
include ../ass_romrules
