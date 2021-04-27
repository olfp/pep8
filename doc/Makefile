#
# pep-8 docs makefile
#

ifeq ($(OS),Windows_NT)
    detected_OS := Windows
else
    detected_OS := $(shell uname -s)
endif

#MANPRE=cat
#ifeq ($(detected_OS),Darwin)  # Mac OS X
#MANPRE=iconv -f utf8 -t latin1
#endif

PAPER=a4
SRC1=pot.1 pepsi.1 teletype.1
SRC5=pot.5
SRC7=pep8.7 sixbit.7
SRC=$(SRC1) $(SRC5) $(SRC7)
PS=pot.1.ps pot.5.ps pepsi.1.ps pep8.7.ps sixbit.7.ps teletype.1.ps
PDF=pot.1.pdf pot.5.pdf pepsi.1.pdf pep8.7.pdf sixbit.7.pdf teletype.1.pdf

%.pdf: %
	groff -k -t -T pdf -dpaper=$(PAPER) -man $< > $@

all: $(PDF)

distclean:
	rm -f $(PDF)

install:
	cp $(SRC1) /usr/local/man/man1
	cp $(SRC5) /usr/local/man/man5
	cp $(SRC7) /usr/local/man/man7



