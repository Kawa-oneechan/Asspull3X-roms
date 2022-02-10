export TARGET :=	midiboard
export SFILES :=	crt0.s piano.s tiny.s 
export CFILES :=	main.c
export AFILES :=	
include ../ass_romrules

# --------
# GRAPHICS
# --------

export IMG2ASS := python ../img2ass.py

piano.o : piano.s piano.png
	$(IMG2ASS) piano.png
	$(AS) $(ASFLAGS) -c $< -o $@
