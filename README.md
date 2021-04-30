pep8
Practical Education Processor based on pdp-8

Includes a macro assembler, a simulator and several devices, including a virtual teletype. An emulated scope display is available seperately (see scope dir).

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

Full example: Hello world for pep8
```
;
; pep8 hello world
; this example requires device 0 (teletype)
;

TTY=	0			; device id for teletype
SAR=	CLL RAR			; make shift right

RESET:	JMP	START

; the text to output is counted packed sixbit code, 
; so two characters per word preceeded by a count word

HELO:	TEXT	/Hello World!/ 	; counted packed sixbit
WCNT:	0			; counter for words

ACR:	15			; CR 
ALF:	12			; LF

AOFF:	40			; offset sixbit -> ASCII
MSK6:	77			; lower sixbit mask

PTR:	(HELO+1)		; pointer to current word

; now start

START:	RCL	HELO
	STO	WCNT		; countdown words sent

LOOP:	JMS	CHKLEN		; check if done
	RCL I	PTR		; get double-char
	SAR			; isolate high six bit
	SAR
	SAR
	SAR
	SAR
	SAR
	CLL			; asure link zero
	TAD	AOFF		; ASCII offset
	JMS	CHOUT		; send to TTY

	JMS	CHKLEN		; are we done midword?
	RCL I	PTR		; re-get double char
	AND	MSK6		; isolate lower six bit
	CLL			; asure link zero
	TAD	AOFF		; ASCII offset
	JMS	CHOUT		; send to TTY

	RCL	PTR		; advance to next word
	IAC
	STO	PTR

	JMP	LOOP

FINIS:	RCL	ACR		; send CR to TTY
	JMS	CHOUT
	RCL	ALF		; send LF to TTY
	JMS	CHOUT

	HLT			; we're done

CHKLEN:	NOP			; slot for return addr
	RCL	WCNT		; remaining length 
	SNA			; none? 
	JMP	FINIS		; then done
	CMA IAC			; else complement, increment
	CMA			; complement again 
	STO	WCNT		; now its decemeneted
	JMP I	CHKLEN		; return

CHOUT:	NOP			; slot for return addr
WAIT:	SRO	TTY		; skip if tty ready
	JMP	WAIT		; else loop
	DPA	TTY		; print char in AC
	JMP I	CHOUT		; return

; EOF
```

Credits: Thank you for the GPIO and tm16437 support code

* joan2937 for the pigpio library (http://abyz.me.uk/rpi/pigpio/)
* Johannes Braun for the tm1637 driver code (https://github.com/hannenz/tm1637)
