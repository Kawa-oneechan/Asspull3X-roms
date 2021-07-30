export TARGET :=	tetris
export SFILES :=	crt0.s tiles.s background.s
export CFILES :=	main.c music.c controller.c drawer.c game.c grid.c helpers.c tetramino.c ../ass-rt.c
export AFILES :=	
include ../ass_romrules

# --------
# GRAPHICS
# --------

export IMG2ASS ?= ../img2ass.exe
export GRIT := $(DEVKITARM)/bin/grit.exe
GRITFLAGS := -ftb -fh! -gB4

tiles.o : tiles.s title.png tiles.png farah.png logo.png
	$(IMG2ASS) title.png
# Tiles are 8x8
	$(GRIT) tiles.png $(GRITFLAGS)
# Farah is a mess (TODO)
	$(GRIT) farah.png $(GRITFLAGS)
# Logo is 32x32, doesn't have a unique palette
	$(GRIT) logo.png $(GRITFLAGS) -p! -Mw4 -Mh4
# Flip palette data around
	$(OBJCOPY) -F binary --reverse-bytes=2 tiles.pal.bin
	$(OBJCOPY) -F binary --reverse-bytes=2 farah.pal.bin
# Produce final result
	$(AS) $(ASFLAGS) -c $< -o $@
