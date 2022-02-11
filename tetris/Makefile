export TARGET :=	tetris
export SFILES :=	crt0.s tiles.s background.s
export CFILES :=	main.c imfplay.c controller.c drawer.c game.c grid.c helpers.c tetramino.c ../ass-rt.c
export AFILES :=	
include ../ass_romrules

# --------
# GRAPHICS
# --------

GRITFLAGS := -ftb -fh! -gB4

tiles.s : title.api tiles.img.bin farah.img.bin logo.img.bin
	$(IMG2ASS) title.png

title.api : title.png
	$(IMG2ASS) title.png

# Tiles are 8x8
tiles.img.bin : tiles.png
	$(GRIT) tiles.png $(GRITFLAGS)
	$(OBJCOPY) -F binary --reverse-bytes=2 tiles.pal.bin

# Farah is 32x32
farah.img.bin : farah.png
	$(GRIT) farah.png $(GRITFLAGS) -Mw4 -Mh4
	$(OBJCOPY) -F binary --reverse-bytes=2 farah.pal.bin

# Logo is 32x32, doesn't have a unique palette
logo.img.bin : logo.png
	$(GRIT) logo.png $(GRITFLAGS) -p! -Mw4 -Mh4

background.s : background.json
	$(TILED2ASS) background.json
