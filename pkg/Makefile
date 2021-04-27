ARCH := $(shell dpkg-architecture -q DEB_TARGET_GNU_TYPE)
debian: 
	mkdir -p pep8/DEBIAN
	cp DEBIAN-control  pep8/DEBIAN/control
	echo 'Architecture: ' $(ARCH) >> pep8/DEBIAN/control
	mkdir -p pep8/usr/local/bin
	mkdir -p pep8/usr/local/man/man1
	mkdir -p pep8/usr/local/man/man1
	mkdir -p pep8/usr/local/man/man7
	cp ../bin/pot ../bin/pepsi ../bin/teletype pep8/usr/local/bin
	cp ../doc/*.1 pep8/usr/local/man/man1
	cp ../doc/*.5 pep8/usr/local/man/man5
	cp ../doc/*.7 pep8/usr/local/man/man7
	dpkg-deb --build pep8 dist
