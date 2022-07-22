GRITS := $(wildcard *.grit)
GRAPHICS := $(GRITS:.grit=.png)
GFXAPI := title.png
TILED := map.json

export TARGET :=	rpg
export SFILES :=	crt0.s graphics.s testmap.s office16.s
export CFILES :=	main.c map.c script.c party.c battle.c vwf.c \
			data1.c \
			$(GRAPHICS:.png=.c) ../ass-rt.c
include ../ass_romrules
