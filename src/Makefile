#
#  Makefile for pdp-8 project
#

VER=VERSION

CC = gcc

ASM = pot
SIM = pepsi

PROGRAMS = $(ASM) $(SIM)

# decide whether to use the GNU readline lib
GNURL     = -DGNURL -DMACRL
GNURLLIBS = -lreadline -ltermcap

CFLAGS =  -ansi -pedantic -g $(GNURL)
#CFLAGS =  -ansi -pedantic -g -D_POSIX_SOURCE $(GNURL)
LDFLAGS = -g
#CFLAGS = -ansi -pedantic -O6 -D_POSIX_SOURCE $(GNURL)
#LDFLAGS = -N -s 

SRC = *.c *.h

DEMOS = *.pps

#DOC = $(VER) *.[1-9n]

ALIBS =
SLIBS = -lpthread  $(GNURLLIBS)
XSLIBS = -lpthread -lsocket -lxnet $(GNURLLIBS)
CLIBS =

ASMOBJS = pot.o
SIMOBJS = pepsi.o display.o execute.o interact.o operate.o chario.o util.o
COMOBJS = octal.o

all:	do-it-all

ifeq (.depend,$(wildcard .depend))
include .depend
do-it-all:      progs
else
do-it-all:      depend progs
endif

progs:	$(PROGRAMS)

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




