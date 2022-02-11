export TARGET :=	tetris
export SFILES :=	crt0.s tiles.s background.s
export CFILES :=	main.c imfplay.c controller.c drawer.c game.c grid.c helpers.c tetramino.c ../ass-rt.c
export AFILES :=	
include ../ass_romrules

# --------
# GRAPHICS
# --------

GRITFLAGS := -ftb -fh! -gB4

tiles.s : title.png tiles.png farah.png logo.png
	$(IMG2ASS) title.png
# Tiles are 8x8
	$(GRIT) tiles.png $(GRITFLAGS)
# Farah is 32x32
	$(GRIT) farah.png $(GRITFLAGS) -Mw4 -Mh4
# Logo is 32x32, doesn't have a unique palette
	$(GRIT) logo.png $(GRITFLAGS) -p! -Mw4 -Mh4
# Flip palette data around
	$(OBJCOPY) -F binary --reverse-bytes=2 tiles.pal.bin
	$(OBJCOPY) -F binary --reverse-bytes=2 farah.pal.bin

background.s : background.json
	$(TILED2ASS) background.json
