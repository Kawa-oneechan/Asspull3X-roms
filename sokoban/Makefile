export TARGET :=	sokoban
export SFILES :=	crt0.s tiles.s hdma.s sound.s
export CFILES :=	main.c levels.c imfplay.c ../ass-rt.c
export AFILES :=	
include ../ass_romrules

# --------
# GRAPHICS
# --------

GRITFLAGS := -ftb -fh! -gB4
# I'll explain this just here in "Sokoban":
# -ftb    file type binary
# -fh!    no header
# -gB4    tile data is 4 bpp (no matter the image bitdepth, the system only supports 4bpp tiles)

tiles.s : title.api tiles.img.bin font.img.bin player.img.bin diskette.img.bin backs.img.bin

title.api : title.png
	$(IMG2ASS) title.png

# Tiles are 16x16
tiles.img.bin : tiles.png
	$(GRIT) tiles.png $(GRITFLAGS) -Mw2 -Mh2
	$(OBJCOPY) -F binary --reverse-bytes=2 tiles.pal.bin

# Font is 8x8 (default), we don't need a palette.
font.img.bin : font.png
	$(GRIT) font.png $(GRITFLAGS) -p!

# Backgrounds are 32x32
backs.img.bin : backs.png
	$(GRIT) backs.png $(GRITFLAGS) -Mw4 -Mh4 -p!

# Player sprites are 16x32
player.img.bin : player.png
	$(GRIT) player.png $(GRITFLAGS) -Mw2 -Mh4
	$(OBJCOPY) -F binary --reverse-bytes=2 player.pal.bin

# Spinning diskette is a bitmap
diskette.img.bin : diskette.png
	$(GRIT) diskette.png $(GRITFLAGS) -gb
	$(OBJCOPY) -F binary --reverse-bytes=2 diskette.pal.bin
