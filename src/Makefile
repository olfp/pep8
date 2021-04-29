#
#  Makefile for pdp-8 project
#

# GPIO = No

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	ifeq ($(GPIO),No)
		HW := disabled
	else
		HW := $(shell cat /proc/cpuinfo | awk -F: '/Hardware/ {gsub(/[ 0-9]/, "",$$2); print $$2}')
	endif
else
	HW := other
endif

ifeq ($(HW),BCM)
	RASPI = true
	RASPIDEFS = -D__raspi__
	RASPILIBS = -lpigpio
	RASPIOBJS = raspi/tm1637.o
	EXTRAS = $(RASPIOBJS)
endif

VER=VERSION

CC = gcc

BINDIR=../bin

ASM = $(BINDIR)/pot
SIM = $(BINDIR)/pepsi


PROGRAMS = $(ASM) $(SIM)

# decide whether to use the GNU readline lib
oldGNURL  = -DGNURL -DMACRL
GNURL     = -DGNURL
GNURLLIBS = -lreadline -ltermcap

CFLAGS = -pedantic -g $(GNURL) $(RASPIDEFS)
#CFLAGS =  -ansi -pedantic -g -D_POSIX_SOURCE $(GNURL)
LDFLAGS = -g
#CFLAGS = -ansi -pedantic -O6 -D_POSIX_SOURCE $(GNURL)
#LDFLAGS = -N -s 

SRC = *.c *.h

DEMOS = *.pps

#DOC = $(VER) *.[1-9n]

ALIBS =
SLIBS = -lpthread $(RASPILIBS)  -lm $(GNURLLIBS)
XSLIBS = -lpthread -lsocket -lxnet $(GNURLLIBS)
CLIBS =

ASMOBJS = pot.o
SIMOBJS = pepsi.o display.o execute.o interact.o operate.o chario.o util.o \
	$(RASPIOBJS)
COMOBJS = octal.o strutil.o

all:	do-it-all

ifeq (.depend,$(wildcard .depend))
include .depend
do-it-all:      progs
else
do-it-all:      depend progs
endif

progs:	$(BINDIR) $(EXTRAS) $(PROGRAMS)

$(BINDIR):
	@echo "Folder $(BINDIR) does not exist"
	mkdir -p $@

$(ASM):	$(ASMOBJS) $(COMOBJS)
	$(CC) $(LDFLAGS) -o $(ASM) $(ASMOBJS) $(COMOBJS) $(ALIBS) $(CLIBS)

$(SIM):	$(SIMOBJS) $(COMOBJS)
	$(CC) $(LDFLAGS) -o $(SIM) $(SIMOBJS) $(COMOBJS) $(SLIBS) $(CLIBS)

dclean:
	if [ -f .depend ]; then rm -f .depend; fi

aclean:	dclean
	rm -f *~ core $(ASMOBJS) $(COMOBJS) $(ASM)

sclean:	dclean
	rm -f *~ core $(SIMOBJS) $(COMOBJS) $(SIM)

clean8:
	rm -f *~ *.psy *.pmi

clean:	aclean sclean clean8

depend:	dep

dep:
	@echo checking dependencies ...
	@for s in *.c; do $(CPP) -MM $$s; done >.depend
	@ echo ... done.

install:
	install -m 755 $(ASM) /usr/local/bin
	install -m 755 $(SIM) /usr/local/bin

tar:
	@echo creating archive ...
	tar cfz pep8-proj-`date '+%y%m%d'`a.tar.gz Makefile $(SRC) $(DEMOS) $(DOC)




