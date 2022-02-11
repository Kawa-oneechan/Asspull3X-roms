export TARGET :=	salanyander
export SFILES :=	crt0.s graphics.s bg_space.s
export CFILES :=	main.c actor.c act-player.c act-starfield.c ../ass-rt.c
export AFILES :=	
include ../ass_romrules

# --------
# GRAPHICS
# --------

GRITFLAGS := -ftb -fh! -gB4

graphics.s : font.png
	$(GRIT) players/vicviper.png $(GRITFLAGS) -Mw4 -Mh2 -oplayers/vicviper.bin
	$(GRIT) players/lordbritish.png $(GRITFLAGS) -Mw4 -Mh2 -oplayers/lordbritish.bin

	$(OBJCOPY) -F binary --reverse-bytes=2 players/vicviper.pal.bin
	$(OBJCOPY) -F binary --reverse-bytes=2 players/lordbritish.pal.bin

	$(GRIT) font.png $(GRITFLAGS)
	$(GRIT) starfield.png $(GRITFLAGS)
	$(OBJCOPY) -F binary --reverse-bytes=2 font.pal.bin
	$(OBJCOPY) -F binary --reverse-bytes=2 starfield.pal.bin

bg_space.s : bg_space.json
	$(TILED2ASS) bg_space.json
