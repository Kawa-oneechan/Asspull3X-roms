export TARGET :=	twohundred
export SFILES :=	crt0.s pic.s
export CFILES :=	main.c ../ass-rt.c
export AFILES :=	
include ../ass_romrules

# --------
# GRAPHICS
# --------

export IMG2ASS := python ../img2ass.py

pic.o : pic.s 200-16.png
	$(IMG2ASS) 200-16.png
	$(AS) $(ASFLAGS) -c $< -o $@
