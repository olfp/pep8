pep8
Practical Education Processor based on pdp-8

Includes an assembler, a simulator and several devices, including a virtual teletype. An emulated scope display is available seperately (see scope dir).

When run on a Raspberry Pi with Pi OS it can use GPIO Pins. To supress building with GPIO support, set edit
the Makefile and remove the comment at line 5 to set GPIO = No.

Binary poackages are built sometimes. Use the source if possible.

To install binaries download release (.deb) and install with "sudo apt install ./pep8-xxx.deb" or "dpkg -i pep8-xxx.deb".
Currently only a Debian style package is supplied.
