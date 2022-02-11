export TARGET :=	salanyander
export SFILES :=	crt0.s graphics.s bg_space.s
export CFILES :=	main.c actor.c act-player.c act-starfield.c ../ass-rt.c
export AFILES :=	
include ../ass_romrules

# --------
# GRAPHICS
# --------

GRITFLAGS := -ftb -fh! -gB4

graphics.s : font.img.bin starfield.img.bin players/vicviper.img.bin
font.img.bin : font.png
	$(GRIT) font.png $(GRITFLAGS)
	$(OBJCOPY) -F binary --reverse-bytes=2 font.pal.bin
starfield.img.bin : starfield.png
	$(GRIT) starfield.png $(GRITFLAGS)
	$(OBJCOPY) -F binary --reverse-bytes=2 starfield.pal.bin

players/vicviper.img.bin : players/vicviper.png
	$(GRIT) players/vicviper.png $(GRITFLAGS) -Mw4 -Mh2 -oplayers/vicviper.bin
	$(GRIT) players/lordbritish.png $(GRITFLAGS) -Mw4 -Mh2 -oplayers/lordbritish.bin
	$(OBJCOPY) -F binary --reverse-bytes=2 players/vicviper.pal.bin
	$(OBJCOPY) -F binary --reverse-bytes=2 players/lordbritish.pal.bin

bg_space.s : bg_space.json
	$(TILED2ASS) bg_space.json
