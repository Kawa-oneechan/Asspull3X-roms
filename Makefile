SUBDIRS:= `ls | egrep -v '^(CVS)$$'`

all:
	@for i in $(SUBDIRS); do if test -e $$i/Makefile ; then $(MAKE) -C $$i || { exit 1;} fi; done;
	./bfi -f=disk1.img _disk1_
	./bfi -f=disk2.img _disk2_
	python -c "f=open('disk1.img','rb');s=f.read();f.close();s=s.replace(b'240-16  API\x00',b'240-16  API\x20');s=s.replace(b'HELLO   APP\x00',b'HELLO   APP\x01');s=s.replace(b'_CHARMAPTXT\x00',b'_CHARMAPTXT\x02');f=open('disk1.img','wb');f.write(s);f.close();"

clean:
	@for i in $(SUBDIRS); do if test -e $$i/Makefile ; then $(MAKE) -C $$i clean || { exit 1;} fi; done;
