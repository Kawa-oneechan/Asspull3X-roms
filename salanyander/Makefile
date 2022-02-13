GFX8x8 := font.png starfield.png
GFX32x16 := $(wildcard players/*.png)
TILED := bg_space.json

export TARGET :=	salanyander
export SFILES :=	crt0.s $(TILED:.json=.s)
export CFILES :=	main.c actor.c act-player.c act-starfield.c $(GFX8x8:.png=.c) $(GFX32x16:.png=.c) ../ass-rt.c
include ../ass_romrules
