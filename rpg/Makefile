export TARGET :=	rpg
export SFILES :=	crt0.s graphics.s testmap.s office16.s
export CFILES :=	main.c ../ass-rt.c
export AFILES :=	
include ../ass_romrules

# --------
# GRAPHICS
# --------

export IMG2ASS ?= ../img2ass.exe
export GRIT := $(DEVKITARM)/bin/grit.exe
GRITFLAGS := -ftb -fh! -gB4

graphics.o : graphics.s title.png font.png farah.png spritepal.png
	$(IMG2ASS) title.png
# Font is 8x16, other UI stuff isn't really but lol
	$(GRIT) font.png $(GRITFLAGS) -Mh2
# Player sprites are 16x24
	$(GRIT) farah.png $(GRITFLAGS) -Mw2 -Mh3 -p!
	$(GRIT) ilira.png $(GRITFLAGS) -Mw2 -Mh3 -p!
	$(GRIT) spritepal.png -ftb -fh! -g! -gb -pe96
# Flip palette data around
	$(OBJCOPY) -F binary --reverse-bytes=2 font.pal.bin
	$(OBJCOPY) -F binary --reverse-bytes=2 spritepal.pal.bin
# Produce final result
	$(AS) $(ASFLAGS) -c $< -o $@

office16.o : office16.s office8.png
	$(GRIT) office8.png $(GRITFLAGS)
	$(OBJCOPY) -F binary --reverse-bytes=2 office8.pal.bin
	$(AS) $(ASFLAGS) -c $< -o $@
