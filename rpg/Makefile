export TARGET :=	rpg
export SFILES :=	crt0.s graphics.s testmap.s office16.s
export CFILES :=	main.c ../ass-rt.c
export AFILES :=	
include ../ass_romrules

# --------
# GRAPHICS
# --------

GRITFLAGS := -ftb -fh! -gB4

graphics.s : title.api font.img.bin spritepal.pal.bin farah_prt.img.bin

title.api : title.png
	$(IMG2ASS) title.png

# Font is 8x16, other UI stuff isn't really but lol
font.img.bin: font.png
	$(GRIT) font.png $(GRITFLAGS) -Mh2
	$(OBJCOPY) -F binary --reverse-bytes=2 font.pal.bin

# Player sprites are 16x24
spritepal.pal.bin : spritepal.png
	$(GRIT) farah.png $(GRITFLAGS) -Mw2 -Mh3 -p!
	$(GRIT) ilira.png $(GRITFLAGS) -Mw2 -Mh3 -p!
	$(GRIT) spritepal.png -ftb -fh! -g! -gb -pe96
	$(OBJCOPY) -F binary --reverse-bytes=2 spritepal.pal.bin

# Portraits are 32x32, but are just the one per file.
farah_prt.img.bin : farah_prt.png
	$(GRIT) farah_prt.png $(GRITFLAGS)
	$(GRIT) ilira_prt.png $(GRITFLAGS)
	$(OBJCOPY) -F binary --reverse-bytes=2 farah_prt.pal.bin
	$(OBJCOPY) -F binary --reverse-bytes=2 ilira_prt.pal.bin

office16.s : office8.img.bin

office8.img.bin : office8.png
	$(GRIT) office8.png $(GRITFLAGS)
	$(OBJCOPY) -F binary --reverse-bytes=2 office8.pal.bin

