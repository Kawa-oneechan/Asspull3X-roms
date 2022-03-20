GFX8x8FP := office8.png 
GFX8x16 := font.png
GFX32x32 := farah_prt.png ilira_prt.png
GFXAPI := title.png
HDMA := hdma1.png
TILED := map.json

GFX16x24NP := farah.png ilira.png
GFXpal := spritepal.png

export TARGET :=	rpg
export SFILES :=	crt0.s graphics.s testmap.s office16.s
export CFILES :=	main.c $(GFX8x8FP:.png=.c) $(GFX8x16:.png=.c) $(GFX32x32:.png=.c) $(GFX16x24NP:.png=.c) $(GFXpal:.png=.c)  ../ass-rt.c
include ../ass_romrules
