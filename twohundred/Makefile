export TARGET :=	twohundred
export SFILES :=	crt0.s pic.s
export CFILES :=	main.c ../ass-rt.c
export AFILES :=	
include ../ass_romrules

# --------
# GRAPHICS
# --------

pic.s : 200-16.api
200-16.api : 200-16.png
	$(IMG2ASS) 200-16.png
