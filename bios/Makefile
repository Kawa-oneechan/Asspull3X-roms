PATH		:= $(DEVKIT68K)/bin:$(PATH)
CROSS		:= m68k-elf-
CC		:= $(CROSS)gcc
AS		:= $(CROSS)gcc
LD		:= $(CROSS)gcc
OBJCOPY		:= $(CROSS)objcopy

IMG2ASS		:= python ../img2ass.py
HDMA2ASS	:= python ../hdma2ass.py
GRIT		:= $(DEVKITARM)/bin/grit.exe

TARGET		:= ass-bios

# Regular makefile continues here
SFILES	:=	crt0.s font.s splash.s hdma.s icons.s
CFILES	:=	main.c f_text.c f_draw.c f_misc.c f_disk.c \
		../ass-rt.c printf.c ff.c diskio.c 

export OFILES	:= $(SFILES:.s=.o) $(CFILES:.c=.o)

# --------------------------------------------------------------------
# OPTIONS
# --------------------------------------------------------------------

MAPFILE		:= $(TARGET).map

ARCH		:= -m68000
SPECS		:= -specs=asspull-bios.specs

INCLUDE		:= -I toolinclude
LIBPATHS	:=

CFLAGS		:= $(INCLUDE) -Wall -Os -fno-strict-aliasing -fomit-frame-pointer $(ARCH)
ASFLAGS		:= -x assembler-with-cpp -c $(ARCH) -Wall
LDFLAGS		:= $(ARCH) $(SPECS) $(LIBPATHS) $(LIBS)

LDFLAGS += -Wl,-Map $(MAPFILE) -nostartfiles

# ---------------------------------------------------------------------
# RULES
# ---------------------------------------------------------------------

../$(TARGET).apb : $(TARGET).elf
	$(OBJCOPY) -O binary $< $@
#	$(CROSS)objdump -d $(TARGET).elf > $(TARGET).txt

$(TARGET).elf : $(OFILES) $(AFILES) Makefile $(DEPENDFILE)
	@echo > $(MAPFILE)
	$(LD) -g -o $@ $(OFILES) $(AFILES) $(LDFLAGS)

.PHONY: all clean depend

all:    clean $(TARGET).elf

clean:
	rm -f $(OFILES) $(MAPFILE) $(TARGET).elf *.img.bin *.pal.bin *.api
	# ../$(TARGET).apb

# --- ROM compilation ---
%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

# --- Assembling ---
%.o : %.s
	$(AS) $(ASFLAGS) -c $< -o $@

# --------------------------------------------------------------------
# GRAPHICS
# --------------------------------------------------------------------

splash.s : splash.api
splash.api : splash.png
	$(IMG2ASS) splash.png

icons.s : icons.img.bin
icons.img.bin : icons.png
	$(GRIT) icons.png -ftb -fh! -gB4
	$(OBJCOPY) -F binary --reverse-bytes=2 icons.pal.bin

font.s : font1.img.bin font2.img.bin
font1.img.bin : font.png
	$(GRIT) font.png -ofont1 -ab256 -ftb -fh! -p! -gB1
font2.img.bin : font.png
	$(GRIT) font.png -ofont2 -at256 -ftb -fh! -p! -gB1 -th16

hdma.s : hdma1.png hdma2.png
	$(HDMA2ASS) hdma1.png hdma.s hdma1
	$(HDMA2ASS) hdma2.png hdma.s hdma2 -a
