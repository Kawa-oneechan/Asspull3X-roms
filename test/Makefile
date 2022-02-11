export TARGET :=	test
export SFILES :=	crt0.s pictures.s
export CFILES :=	main.c text.c graphics.c keyboard.c mouse.c joypad.c clock.c pcm.c opl.c imfplay.c ../ass-rt.c
export AFILES :=	
include ../ass_romrules

# --------
# GRAPHICS
# --------

GRITFLAGS := -ftb -fh! -gB4

pictures.s : pointer.png bmp320x200x4.png bmp320x240x4.png bmp640x480x4.png bmp320x200x8.png bmp320x240x8.png bmp640x480x8.png
	$(GRIT) pointer.png $(GRITFLAGS) -Mw2 -Mh2 -p!
	$(IMG2ASS) bmp320x200x4.png
	$(IMG2ASS) bmp320x240x4.png
	$(IMG2ASS) bmp640x480x4.png
	$(IMG2ASS) bmp320x200x8.png
	$(IMG2ASS) bmp320x240x8.png
	$(IMG2ASS) bmp640x480x8.png
