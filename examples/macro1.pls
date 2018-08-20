0000:       0000	;
0000:       0000	; pot8 MARCO Test/Design
0000:       0000	;
0000:       0000		MACRO CAA	; complement A and add arg 
0000:       0000		CLL CMA IAC	; make two's complement
0000:       0000		TAD	\1	; add arg
0000:       0000		ENDM
0000:       0000	
0000:       0000		MACRO JML	; jump via link
0000:       0000		JMP I (.+1)	; indirect via next word
0000:       0000		\1		; link is here
0000:       0000		ENDM
0000:       0000	
0000: M     0000	RESET:	JML	START
0000:       5401		JMP I (.+1)	; indirect via next word
0001:       0200		START; link is here
0002:       0000	
0002:       0042	A:	42		; dec 34
0003:       0013	B:	13		; dec 11
0004:       0000	C:	0		; result for c = a - b = (-b) + a = 27 (dec 23) 
0005:       0000	
0200:       0000		PAGE
0200:       0000	
0200:       7100	START:	CLL
0201:       0003		RCL	B
0202: M     0000		CAA	A
0202:       7141		CLL CMA IAC	; make two's complement
0203:       4002		TAD	A 	; add arg
0204:       1004		STO	C
0205:       0000	
0205:       7401	END:	HLT
0206:       0000	
0206:       0000	; EOF

