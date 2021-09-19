export TARGET :=	salanyander
export SFILES :=	crt0.s graphics.s
export CFILES :=	main.c ../ass-rt.c
export AFILES :=	
include ../ass_romrules

# --------
# GRAPHICS
# --------

export IMG2ASS ?= ../img2ass.exe
export GRIT := $(DEVKITARM)/bin/grit.exe
GRITFLAGS := -ftb -fh! -gB4

graphics.o : graphics.s font.png player.png
	$(GRIT) player.png $(GRITFLAGS) -Mw4 -Mh2
	$(GRIT) font.png $(GRITFLAGS)
	$(OBJCOPY) -F binary --reverse-bytes=2 player.pal.bin
	$(OBJCOPY) -F binary --reverse-bytes=2 font.pal.bin
	$(AS) $(ASFLAGS) -c $< -o $@
