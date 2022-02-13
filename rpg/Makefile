GFX8x8 := office8.png 
GFX8x16 := font.png
GFX32x32 := farah_prt.png ilira_prt.png
GFXAPI := title.png
HDMA := hdma1.png
TILED := map.json

GFX16x24 := farah.png ilira.png

export TARGET :=	rpg
export SFILES :=	crt0.s graphics.s testmap.s office16.s
export CFILES :=	main.c $(GFX8x8:.png=.c) $(GFX8x16:.png=.c) $(GFX32x32:.png=.c) $(GFX16x24:.png=.c) $(GFXpal:.png=.c)  ../ass-rt.c
include ../ass_romrules

$(GFXpal:.png=.c) : %.c : %.png
	$(GRIT) $< -ftb -fh! -g! -gb -pe96
	
$(GFX16x24:.png=.c) : %.c : %.png
	$(GRIT) $< $(GRITFLAGS) -Mw2 -Mh3 !p -o$@
