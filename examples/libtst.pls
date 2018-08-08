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
0000:       0000		STO TSBS%%	; save ac
0000:       0000		RCL \1		; value to subtract
0000:       0000		CLL CMA IAC	; two's coomplement
0000:       0000		TAD TSBS%%	; add saved val
0000:       0000		SKP
0000:       0000	TSBS%%:	0
0000:       0000		ENDM
0000:       0000		
0000:       0000	; TAC Macros	
0000:       0000		
0000:       0000		MACRO _MOV	; TAC: MOV a b
0000:       0000		RAL \1
0000:       0000		STO \2
0000:       0000		ENDM
0000:       0000		
0000:       0000		MACRO _MOVI	; TAC: mov #a b
0000:       0000	;	SKP
0000:       0000	;	\1
0000:       0000		ZMEM z1 \1
0000:       0000	;	RCL (.-1)
0000:       0000		RCL z1
0000:       0000		STO \2
0000:       0000		ENDM
0000:       0000		
0000:       0000		MACRO _ADD	; TAC: ADD s1 s2 d
0000:       0000		RCL \1
0000:       0000		TAD \2
0000:       0000		STO \3
0000:       0000		ENDM
0000:       0000	
0000:       0000		MACRO _ADDI	; TAC: ADD s1 #s2 d
0000:       0000		SKP
0000:       0000	IMAD%%:	\2
0000:       0000		RCL \1
0000:       0000		TAD IMAD%%
0000:       0000		STO \3
0000:       0000		ENDM
0000:       0000	; EOF
0000:       0000	
0000: M     0000	reset:	jml start
0000:       5401	        JMP I (.+1)     ; indirect via next word
0001:       0200	        START           ; link is here
0002:       0000	
0002:       0042	a:	42
0003:       0013	b:	13
0004:       0000	c:	0
0005:       0000	
0005:       0000	ssix=	66
0005:       0000	
0200:       0000		page
0200:       0000	
0200:       0002	start:	rcl a
0201: M     0000		tsb b
0201:       1206		STO TSBS01	; save ac
0202:       0003		RCL B 		; value to subtract
0203:       7141		CLL CMA IAC	; two's coomplement
0204:       4206		TAD TSBS01	; add saved val
0205:       7410		SKP
0206:       0000	TSBS01:	0
0207:       1004		sto c
0210:       0000			
0210:       0002	again:	rcl a
0211: M     0000		tsb b
0211:       1216		STO TSBS02	; save ac
0212:       0003		RCL B 		; value to subtract
0213:       7141		CLL CMA IAC	; two's coomplement
0214:       4216		TAD TSBS02	; add saved val
0215:       7410		SKP
0216:       0000	TSBS02:	0
0217:       1004		sto c
0220:       0000		
0220: M     0000		_MOVI ssix a
0220:       0000	;	SKP
0220:       0000	;	\1
0220:       0000		ZMEM z1 SSIX
0220:       0000	;	RCL (.-1)
0220:       0177		RCL z1
0221:       1002		STO A 
0222: M     0000		_MOVI 33 b
0222:       0000	;	SKP
0222:       0000	;	\1
0222:       0000		ZMEM z1 33
0222:       0000	;	RCL (.-1)
0222:       0200		RCL z1
0223:       1003		STO B 
0224: M     0000		_ADD  a b c
0224:       0002		RCL A 
0225:       4003		TAD B 
0226:       1004		STO C 
0227: M     0000		_ADDI c 42 a
0227:       7410		SKP
0230:       0042	IMAD01:	42
0231:       0004		RCL C 
0232:       4230		TAD IMAD01
0233:       1002		STO A 
0234:       0000	
0234:       7402		hlt
0235:       0000	
0235:       0000	; eof

