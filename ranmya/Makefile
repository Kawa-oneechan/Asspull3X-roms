export TARGET :=	ranmya
export SFILES :=	crt0.s pic.s
export CFILES :=	main.c ../ass-rt.c
export AFILES :=	
include ../ass_romrules

# --------
# GRAPHICS
# --------

export IMG2ASS := python ../img2ass.py
export APIFILES := 

pic.o : pic.s ranmya.png
	$(IMG2ASS) ranmya.png
	$(AS) $(ASFLAGS) -c $< -o $@
