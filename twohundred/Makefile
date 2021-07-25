export TARGET :=	twohundred
export SFILES :=	crt0.s pic.s
export CFILES :=	main.c ../ass-rt.c
export AFILES :=	
include ../ass_romrules

# --------
# GRAPHICS
# --------

export IMG2ASS ?= ../img2ass.exe
export APIFILES := 200-16.png

pic.o : pic.s $(APIFILES:.png=.api)
	$(AS) $(ASFLAGS) -c $< -o $@

%.api : %.png
	$(IMG2ASS) $<
