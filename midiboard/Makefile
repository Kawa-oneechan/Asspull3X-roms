export TARGET :=	midiboard
export SFILES :=	crt0.s piano.s tiny.s 
export CFILES :=	main.c
export AFILES :=	
include ../ass_romrules

# --------
# GRAPHICS
# --------

piano.s : piano.png
	$(IMG2ASS) piano.png
