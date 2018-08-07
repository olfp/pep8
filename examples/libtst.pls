0000:       0000	;
0000:       0000	; maclib test suite
0000:       0000	;
0000:       0000	
0000:       0000		file maclib
0000:       0000	;
0000:       0000	; first run at a prp8 macro lib
0000:       0000	;
0000:       0000	
0000:       0000	        MACRO JML       ; jump via link
0000:       0000	        JMP I (.+1)     ; indirect via next word
0000:       0000	        \1              ; link is here
0000:       0000	        ENDM
0000:       0000	
0000:       0000		MACRO TSB	; thirteen bit subtract
0000:       0000		STO (.+5)	; save ac
0000:       0000		RCL \1		; value to subtract
0000:       0000		CLL CMA IAC	; two's coomplement
0000:       0000		TAD (.+2)	; add saved val
0000:       0000		SKP
0000:       0000		0
0000:       0000		ENDM
0000:       0000	
0000:       0000	; EOF
0000:       0000	
0000: M     0000	reset:	jml start
0000:       5401	        JMP I (.+1)     ; indirect via next word
0001:       0200	        \1              ; link is here
0002:       0000	
0002:       0042	a:	42
0003:       0013	b:	13
0004:       0000	c:	0
0005:       0000	
0200:       0000		page
0200:       0000	
0200:       0002	start:	rcl a
0201: M     0000		tsb b
0201:       1206		STO (.+5)	; save ac
0202:       0003		RCL \1		; value to subtract
0203:       7141		CLL CMA IAC	; two's coomplement
0204:       4206		TAD (.+2)	; add saved val
0205:       7410		SKP
0206:       0000		0
0207:       1004		sto c
0210:       7402		hlt
0211:       0000	
0211:       0000	; eof

