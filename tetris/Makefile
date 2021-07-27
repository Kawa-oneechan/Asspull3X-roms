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

tiles.o : tiles.s title.png tiles.png farah.png logo.png
	$(IMG2ASS) title.png
	$(GRIT) tiles.png -ftb -fh! -gu16 -gB4 -pu8
	$(GRIT) farah.png -ftb -fh! -gu16 -gB4 -pu8
	$(GRIT) logo.png -ftb -fh! -gu16 -gB4 -p! -Mw4 -Mh4
	$(OBJCOPY) -F binary --reverse-bytes=2 tiles.pal.bin
	$(OBJCOPY) -F binary --reverse-bytes=2 farah.pal.bin
	$(AS) $(ASFLAGS) -c $< -o $@
