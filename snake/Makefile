export TARGET :=	snake
export SFILES :=	crt0.s tiles.s title.s hdma.s
export CFILES :=	main.c
export AFILES :=	
include ../ass_romrules

# --------
# GRAPHICS
# --------

GRITFLAGS := -ftb -fh! -gB4

tiles.s : tiles.img.bin

tiles.img.bin : tiles.png
	$(GRIT) tiles.png $(GRITFLAGS)
	$(OBJCOPY) -F binary --reverse-bytes=2 tiles.pal.bin

hdma.s : hdma.png
	$(HDMA2ASS) hdma.png hdma.s hdma1

title.s : title.json
	$(TILED2ASS) title.json

