export TARGET :=	snake
export SFILES :=	crt0.s tiles.s title.s hdma.s
export CFILES :=	main.c
export AFILES :=	
include ../ass_romrules

# --------
# GRAPHICS
# --------

export HDMA2ASS := python ../hdma2ass.py
export TILED2ASS := python ../tiled2ass.py
export GRIT := $(DEVKITARM)/bin/grit.exe
GRITFLAGS := -ftb -fh! -gB4

tiles.o : tiles.s tiles.png
# Tiles need nothing special...
	$(GRIT) tiles.png $(GRITFLAGS)
# ...except to have the palette data flipped around
	$(OBJCOPY) -F binary --reverse-bytes=2 tiles.pal.bin
	$(AS) $(ASFLAGS) -c $< -o $@

hdma.o : hdma.s hdma.png
	$(HDMA2ASS) hdma.png hdma.s hdma1
	$(AS) $(ASFLAGS) -c $< -o $@

title.o : title.s title.json
	$(TILED2ASS) title.json
	$(AS) $(ASFLAGS) -c $< -o $@

