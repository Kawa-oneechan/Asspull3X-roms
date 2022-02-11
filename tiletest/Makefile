export TARGET :=	tiletest
export SFILES :=	crt0.s tileset.s map.s hdma.s
export CFILES :=	main.c
export AFILES :=	
include ../ass_romrules

# --------
# GRAPHICS
# --------

GRITFLAGS := -ftb -fh! -gB4

tileset.s : tileset.img.bin farah.img.bin

tileset.img.bin : tileset.png question.png grass.png
	$(GRIT) tileset.png $(GRITFLAGS)
	$(GRIT) question.png $(GRITFLAGS) -fp!
	$(GRIT) grass.png $(GRITFLAGS) -fp!
	$(OBJCOPY) -F binary --reverse-bytes=2 tileset.pal.bin

farah.img.bin : farah.png
	$(GRIT) farah.png $(GRITFLAGS)
	$(OBJCOPY) -F binary --reverse-bytes=2 farah.pal.bin

map.s : map.json
	$(TILED2ASS) map.json
