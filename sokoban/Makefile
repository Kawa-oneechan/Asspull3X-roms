GFX8x8NP := font.png
GFX16x16 := tiles.png
GFX16x32 := player.png
GFX32x32 := diskette.png
GFX32x32NP := backs.png
GFXAPI := title.png diskbg.png

export TARGET :=	sokoban
export SFILES :=	crt0.s hdma.s assets.s
export CFILES :=	main.c levels.c ../imfplay.c $(GFXAPI:.png=.c) $(GFX8x8NP:.png=.c) $(GFX16x16:.png=.c) $(GFX16x32:.png=.c) $(GFX32x32:.png=.c) $(GFX32x32NP:.png=.c) $(GFXbmp:.png=.c) ../ass-rt.c
include ../ass_romrules

