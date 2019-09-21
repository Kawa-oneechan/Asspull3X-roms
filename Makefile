export BFI ?= $(DEVKITPRO)/asspull/bfi.exe
SUBDIRS:= `ls | egrep -v '^(CVS)$$'`
all:
	@for i in $(SUBDIRS); do if test -e $$i/Makefile ; then $(MAKE) -C $$i || { exit 1;} fi; done;
	$(BFI) -f=disk1.img _disk1_
	$(BFI) -f=disk2.img _disk2_
