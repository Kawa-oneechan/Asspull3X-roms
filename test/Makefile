GRITS := $(wildcard *.grit)
GRAPHICS := $(GRITS:.grit=.png)
GFXAPI := $(wildcard bmp*.png)

export TARGET :=	test
export SFILES :=	crt0.s assets.s
export CFILES :=	main.c text.c graphics.c keyboard.c mouse.c joypad.c clock.c midi.c pcm.c opl.c ../imfplay.c $(GRAPHICS:.png=.c) $(GFXAPI:.png=.c) ../ass-rt.c
include ../ass_romrules
