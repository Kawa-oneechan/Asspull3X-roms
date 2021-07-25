export TARGET :=	midiboard
export SFILES :=	crt0.s piano.s tiny.s 
export CFILES :=	main.c
export AFILES :=	
include ../ass_romrules

# --------
# GRAPHICS
# --------

export IMG2ASS ?= ../img2ass.exe
export APIFILES := piano.png

piano.o : piano.s $(APIFILES:.png=.api)
	$(AS) $(ASFLAGS) -c $< -o $@

%.api : %.png
	$(IMG2ASS) $<
