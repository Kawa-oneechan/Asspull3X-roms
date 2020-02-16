export TARGET :=	columns
export SFILES :=	crt0.s tiles.s
export CFILES :=	main.c ../ass-rt.c
export AFILES :=	

include ../ass_romrules
# --------
# GRAPHICS
# --------

export GRIT := $(DEVKITARM)/bin/grit.exe

tiles.o : tiles.s tiles.png girl1.png
	$(GRIT) tiles.png -ftb -fh! -gu16 -gB4 -pu8
	$(GRIT) girl1.png -ftb -fh! -gu16 -gB4 -pu8
	$(OBJCOPY) -F binary --reverse-bytes=2 tiles.pal.bin
	$(OBJCOPY) -F binary --reverse-bytes=2 girl1.pal.bin
	$(AS) $(ASFLAGS) -c $< -o $@
