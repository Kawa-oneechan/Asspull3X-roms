GRITS := $(wildcard *.grit)
GRAPHICS := $(GRITS:.grit=.png)

HDMA := hdma1.png
TILED := map.json

export TARGET :=	tiletest
export SFILES :=	crt0.s $(HDMA:.png=.s) $(TILED:.json=.s)
export CFILES :=	main.c smb1as.c $(GRAPHICS:.png=.c)
include ../ass_romrules
