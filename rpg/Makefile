GRITS := $(wildcard *.grit)
GRAPHICS := $(GRITS:.grit=.png)
GFXAPI := title.png
othercleans := database.s testmap.s

export TARGET :=	rpg
export SFILES :=	crt0.s graphics.s testmap.s office16.s database.s
export CFILES :=	main.c map.c script.c party.c battle.c vwf.c \
			$(GRAPHICS:.png=.c) ../ass-rt.c
include ../ass_romrules

database.s : database.json
	python makedb.py database.json

testmap.s : testmap.json
	python makemap.py testmap.json
