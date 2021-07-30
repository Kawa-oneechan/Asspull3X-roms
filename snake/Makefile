export TARGET :=	snake
export SFILES :=	crt0.s tiles.s title.s hdma.s
export CFILES :=	main.c
export AFILES :=	
include ../ass_romrules

# --------
# GRAPHICS
# --------

export GRIT := $(DEVKITARM)/bin/grit.exe
GRITFLAGS := -ftb -fh! -gB4

tiles.o : tiles.s tiles.png
# Tiles need nothing special...
	$(GRIT) tiles.png ($GRITFLAGS)
# ...except to have the palette data flipped around
	$(OBJCOPY) -F binary --reverse-bytes=2 tiles.pal.bin
	$(AS) $(ASFLAGS) -c $< -o $@
