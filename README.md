pep8
Practical Education Processor based on pdp-8

Includes an assembler, a simulator and several devices, including a virtual teletype. An emulated scope display is available seperately (see scope dir).

When run on a Raspberry Pi with Pi OS it can use GPIO Pins. To supress building with GPIO support, set edit
the Makefile and remove the comment at line 5 to set GPIO = No.

Binary poackages are built sometimes. Use the source if possible.

Since pep8 uses pigpio and hannenz/tm1637 for the Raspberry Pi hardware support you need to

1. If not already present, install the pigpio library

```bash
git clone https://github.com/joan2937/pigpio.git
cd pigpio
make clean && make && sudo make install
```

2. Clone the pep8 repsitory in recursive mode to include to tm1737 module

```bash
git clone --recursive https://github.com/olfp/pep8.git
cd pep8
make clean && make && sudo make install
```

when building with GPIO support.

To install binaries download release (.deb) and install with "sudo apt install ./pep8-xxx.deb" or "dpkg -i pep8-xxx.deb".
Currently only a Debian style package is supplied.

Credits: Thank you for the GPIO and tm16437 support code

* joan2937 for the pigpio library (http://abyz.me.uk/rpi/pigpio/)
* Johannes Braun for the tm1637 driver code (https://github.com/hannenz/tm1637)
