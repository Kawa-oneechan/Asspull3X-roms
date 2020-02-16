export TARGET :=	snake
export SFILES :=	crt0.s tiles.s title.s hdma.s
export CFILES :=	main.c
export AFILES :=	
include ../ass_romrules

# --------
# GRAPHICS
# --------

export GRIT := $(DEVKITARM)/bin/grit.exe

tiles.o : tiles.s tiles.png
	$(GRIT) tiles.png -ftb -fh! -gu16 -gB4 -pu8
	$(OBJCOPY) -F binary --reverse-bytes=2 tiles.pal.bin
	$(AS) $(ASFLAGS) -c $< -o $@
