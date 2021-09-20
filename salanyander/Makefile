export TARGET :=	salanyander
export SFILES :=	crt0.s graphics.s starfield-map.s
export CFILES :=	main.c ../ass-rt.c
export AFILES :=	
include ../ass_romrules

# --------
# GRAPHICS
# --------

export IMG2ASS ?= ../img2ass.exe
export GRIT := $(DEVKITARM)/bin/grit.exe
GRITFLAGS := -ftb -fh! -gB4

graphics.o : graphics.s font.png player1.png player2.png
	$(GRIT) player1.png $(GRITFLAGS) -Mw4 -Mh2
	$(GRIT) player2.png $(GRITFLAGS) -Mw4 -Mh2
	$(GRIT) font.png $(GRITFLAGS)
	$(GRIT) starfield.png $(GRITFLAGS)
	$(OBJCOPY) -F binary --reverse-bytes=2 player1.pal.bin
	$(OBJCOPY) -F binary --reverse-bytes=2 player2.pal.bin
	$(OBJCOPY) -F binary --reverse-bytes=2 font.pal.bin
	$(OBJCOPY) -F binary --reverse-bytes=2 starfield.pal.bin
	$(AS) $(ASFLAGS) -c $< -o $@
