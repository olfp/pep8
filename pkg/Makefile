ARCH := $(shell uname -sm | tr ' ' '_')
debian: 
	mkdir -p pep8/usr/local/bin
	mkdir -p pep8/usr/local/man/man1
	mkdir -p pep8/usr/local/man/man1
	mkdir -p pep8/usr/local/man/man7
	cp ../bin/pot ../bin/pepsi ../bin/teletype pep8/usr/local/bin
	cp ../doc/*.1 pep8/usr/local/man/man1
	cp ../doc/*.5 pep8/usr/local/man/man5
	cp ../doc/*.7 pep8/usr/local/man/man7
	dpkg-deb --nocheck --build pep8-$(ARCH)
