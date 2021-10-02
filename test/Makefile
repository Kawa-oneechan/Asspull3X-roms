export TARGET :=	test
export SFILES :=	crt0.s pictures.s
export CFILES :=	main.c text.c keyboard.c bitmaps.c mouse.c imfplay.c
export AFILES :=	
include ../ass_romrules

# --------
# GRAPHICS
# --------

export IMG2ASS ?= ../img2ass.exe
export GRIT := $(DEVKITARM)/bin/grit.exe
GRITFLAGS := -ftb -fh! -gB4

pictures.o : pictures.s pointer.png bmp320x200x4.png bmp320x240x4.png bmp640x480x4.png bmp320x200x8.png bmp320x240x8.png bmp640x480x8.png
	$(GRIT) pointer.png $(GRITFLAGS) -Mw2 -Mh2 -p!
	$(IMG2ASS) bmp320x200x4.png
	$(IMG2ASS) bmp320x240x4.png
	$(IMG2ASS) bmp640x480x4.png
	$(IMG2ASS) bmp320x200x8.png
	$(IMG2ASS) bmp320x240x8.png
	$(IMG2ASS) bmp640x480x8.png
# Produce final result
	$(AS) $(ASFLAGS) -c $< -o $@
