0000:       0000	;
0000:       0000	; iot tests
0000:       0000	;
0000:       0000	
0000:       0000	TTY=	0		; TTY - device # in bits 4-6
0000:       0000	
0000:       7300	LOOP:	CLL CLA
0001:       7600	RETRY:	SRI	TTY
0002:       5001		JMP	RETRY
0003:       7640		DGA	TTY
0004:       7620	WAIT:	SRO	TTY
0005:       5004		JMP	WAIT
0006:       7660		DPA	TTY
0007:       7141		CLL CMA IAC
0010:       4014		TAD 	ESC
0011:       7420		SZA
0012:       5000		JMP 	LOOP
0013:       7401		HLT
0014:       0000	
0014:       0033	ESC:	0033
0015:       0000	
0015:       0000	; EOF

