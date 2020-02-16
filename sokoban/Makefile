export TARGET :=	sokoban
export SFILES :=	crt0.s tiles.s hdma.s sound.s
export CFILES :=	main.c levels.c music.c ../ass-rt.c
export AFILES :=	
include ../ass_romrules

# --------
# GRAPHICS
# --------

export IMG2ASS ?= ../img2ass.exe
export GRIT := $(DEVKITARM)/bin/grit.exe

tiles.o : tiles.s ../sokoban.png tiles.png player.png diskette.png
	$(IMG2ASS) ../sokoban.png
	$(GRIT) tiles.png -ftb -fh! -gu16 -gB4 -pu8
	$(GRIT) player.png -ftb -fh! -gu16 -gB4 -pu8
	$(GRIT) diskette.png -ftb -fh! -gu16 -gb -gB4 -pu8
	$(OBJCOPY) -F binary --reverse-bytes=2 tiles.pal.bin
	$(OBJCOPY) -F binary --reverse-bytes=2 player.pal.bin
	$(OBJCOPY) -F binary --reverse-bytes=2 diskette.pal.bin
	$(AS) $(ASFLAGS) -c $< -o $@
