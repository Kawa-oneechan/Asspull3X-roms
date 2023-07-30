GRITS := $(wildcard *.grit)
GRAPHICS := $(GRITS:.grit=.png)
TILED := bg_space.json

export TARGET :=	salanyander
export SFILES :=	crt0.s $(TILED:.json=.s)
export CFILES :=	main.c actor.c act-player.c act-starfield.c $(GRAPHICS:.png=.c)
include ../ass_romrules
