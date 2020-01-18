PATH	:= $(DEVKIT68K)/bin:$(PATH)
CROSS	:= m68k-elf-
CC	:= $(CROSS)gcc
AS	:= $(CROSS)gcc
LD	:= $(CROSS)gcc
OBJCOPY	:= $(CROSS)objcopy
TARGET	:= ass-bios

# Regular makefile continues here
SFILES	:=	crt0.s font.s splash.s hdma.s icons.s
CFILES	:=	main.c f_text.c f_draw.c f_misc.c f_disk.c \
		../ass-rt.c printf.c ff.c diskio.c 

export AFILES	:= 
export ROFILES	:= $(SFILES:.s=.o) $(CFILES:.c=.o)
export IOFILES	:= $(ICFILES:.c=.o)
export OFILES	:= $(ROFILES) $(IOFILES)

# --------------------------------------------------------------------
# OPTIONS
# --------------------------------------------------------------------

MAPFILE		:= $(TARGET).map

ARCH    := -m68000
RARCH   := -m68000
SPECS   := -specs=asspull-bios.specs

INCLUDE		:= -I toolinclude
LIBPATHS	:=

CBASE   := $(INCLUDE) -Wall -Os -fno-strict-aliasing -fomit-frame-pointer
RCFLAGS := $(CBASE) $(RARCH)
CFLAGS  := $(RCFLAGS)

ASFLAGS := -x assembler-with-cpp -c -m68000 -Wall
LDFLAGS := $(ARCH) $(SPECS) $(LIBPATHS) $(LIBS)

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

depend:
	$(CC) $(RCFLAGS) -M $(RCFILES) > $(DEPENDFILE)
	$(CC) $(ICFLAGS) -M $(ICFILES) > $(DEPENDFILE)

$(DEPENDFILE): 
	$(CC) $(RCFLAGS) -M $(CFILES) $(ICFLAGS) $(ICFILES) > $(DEPENDFILE)


# --- ROM compilation ---
%.o : %.c
	$(CC) $(RCFLAGS) -c $< -o $@

# --- Assembling ---
%.o : %.s
	$(AS) $(ASFLAGS) -c $< -o $@


#$(.IOFILES) : %.o : %.c
#	$(CC) $(ICFLAGS) -c $< -o $@









