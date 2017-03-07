#
# pep-8 docs makefile
#

PAPER=a4
SRC1=pot.1 pepsi.1 teletype.1
SRC5=pot.5
SRC7=pep8.7 sixbit.7
SRC=$(SRC1) $(SRC5) $(SRC7)
PS=pot.1.ps pot.5.ps pepsi.1.ps pep8.7.ps sixbit.7.ps teletype.1.ps

%.ps: %
	groff -t -dpaper=$(PAPER) -man $< > $@
	pstopdf $@

all: $(PS)

clean:
	rm $(PS)

install:
	cp $(SRC1) /usr/local/man/man1
	cp $(SRC5) /usr/local/man/man5
	cp $(SRC7) /usr/local/man/man7



