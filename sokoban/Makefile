export TARGET :=	sokoban
export SFILES :=	crt0.s tiles.s hdma.s sound.s
export CFILES :=	main.c levels.c imfplay.c ../ass-rt.c
export AFILES :=	
include ../ass_romrules

# --------
# GRAPHICS
# --------

export IMG2ASS := python ../img2ass.py
export GRIT := $(DEVKITARM)/bin/grit.exe
GRITFLAGS := -ftb -fh! -gB4
# I'll explain this just here in "Sokoban":
# -ftb    file type binary
# -fh!    no header
# -gB4    tile data is 4 bpp (no matter the image bitdepth, the system only supports 4bpp tiles)

tiles.o : tiles.s *.pal.bin title.png tiles.png font.png player.png diskette.png backs.png
	$(IMG2ASS) title.png
# Tiles are 16x16
	$(GRIT) tiles.png $(GRITFLAGS) -Mw2 -Mh2
# Font is 8x8 (default), we don't need a palette.
	$(GRIT) font.png $(GRITFLAGS) -p!
# Backgrounds are 32x32
	$(GRIT) backs.png $(GRITFLAGS) -Mw4 -Mh4 -p!
# Player sprites are 16x32
	$(GRIT) player.png $(GRITFLAGS) -Mw2 -Mh4
# Spinning diskette is a bitmap
	$(GRIT) diskette.png $(GRITFLAGS) -gb
# Flip palette data around
	$(OBJCOPY) -F binary --reverse-bytes=2 tiles.pal.bin
	$(OBJCOPY) -F binary --reverse-bytes=2 player.pal.bin
	$(OBJCOPY) -F binary --reverse-bytes=2 diskette.pal.bin
# Produce final result
	$(AS) $(ASFLAGS) -c $< -o $@
