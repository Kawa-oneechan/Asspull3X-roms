export TARGET :=	columns
export SFILES :=	crt0.s tiles.s
export CFILES :=	main.c ../ass-rt.c
include ../ass_romrules

# --------
# GRAPHICS
# --------

export GRIT := $(DEVKITARM)/bin/grit.exe

tiles.s : tiles.img.bin girl1.img.bin

tiles.img.bin : tiles.png
	$(GRIT) tiles.png -ftb -fh! -gu16 -gB4 -pu8
	$(OBJCOPY) -F binary --reverse-bytes=2 tiles.pal.bin
girl1.img.bin : girl1.png
	$(GRIT) girl1.png -ftb -fh! -gu16 -gB4 -pu8
	$(OBJCOPY) -F binary --reverse-bytes=2 girl1.pal.bin
