GFX16x16 := pointer.png
GFXAPI := $(wildcard bmp*.png)

export TARGET :=	test
export SFILES :=	crt0.s assets.s
export CFILES :=	main.c text.c graphics.c keyboard.c mouse.c joypad.c clock.c pcm.c opl.c ../imfplay.c $(GFX16x16:.png=.c) $(GFXAPI:.png=.c) ../ass-rt.c
include ../ass_romrules
