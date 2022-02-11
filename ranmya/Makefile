export TARGET :=	ranmya
export SFILES :=	crt0.s pic.s
export CFILES :=	main.c ../ass-rt.c
export AFILES :=	
include ../ass_romrules

# --------
# GRAPHICS
# --------

pic.s : ranmya.png
	$(IMG2ASS) ranmya.png
