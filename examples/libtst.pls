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
0000:       0000		MACRO _MOVI_	; TAC: mov #a b without ZMEM
0000:       0000		SKP
0000:       0000		\1
0000:       0000		RCL (.-1)
0000:       0000		STO \2
0000:       0000		ENDM
0000:       0000		
0000:       0000		
0000:       0000		MACRO _MOVI	; TAC: mov #a b
0000:       0000		ZMEM TMI%% \1
0000:       0000		RCL TMI%%
0000:       0000		STO \2
0000:       0000		ENDM
0000:       0000		
0000:       0000		MACRO _ADD	; TAC: ADD s1 s2 d
0000:       0000		RCL \1
0000:       0000		TAD \2
0000:       0000		STO \3
0000:       0000		ENDM
0000:       0000	
0000:       0000		MACRO _ADDI_	; TAC: ADD s1 #s2 d without ZMEM
0000:       0000		SKP
0000:       0000	IMAD%%:	\2
0000:       0000		RCL \1
0000:       0000		TAD IMAD%%
0000:       0000		STO \3
0000:       0000		ENDM
0000:       0000	
0000:       0000		MACRO _ADDI	; TAC: ADD s1 #s2 d
0000:       0000		ZMEM TAI%% \2
0000:       0000		RCL \1
0000:       0000		TAD TAI%%
0000:       0000		STO \3
0000:       0000		ENDM
0000:       0000	
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
0005:       0000	ssev=	67
0005:       0000	
0175:       0042	TAI01:	0042
0176:       0033	TMI02:	0033
0177:       0000	TAI02:	0066
0177:       0066	TMI01:	0066
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
0220: M     0000		_MOVI_ ssix a
0220:       7410		SKP
0221:       0066		SSIX
0222:       0221		RCL (.-1)
0223:       1002		STO A 
0224: M     0000		_MOVI_ 33 b
0224:       7410		SKP
0225:       0033		33
0226:       0225		RCL (.-1)
0227:       1003		STO B 
0230: M     0000		_ADD  a b c
0230:       0002		RCL A 
0231:       4003		TAD B 
0232:       1004		STO C 
0233:       0000	
0233: M     0000		_MOVI ssix a
0233:       0000		ZMEM TMI01 SSIX
0233:       0177		RCL TMI01
0234:       1002		STO A 
0235: M     0000		_MOVI 33 b
0235:       0000		ZMEM TMI02 33
0235:       0176		RCL TMI02
0236:       1003		STO B 
0237: M     0000		_ADD  a b c
0237:       0002		RCL A 
0240:       4003		TAD B 
0241:       1004		STO C 
0242:       0000	
0242: M     0000		_ADDI c 42 a
0242:       0000		ZMEM TAI01 42
0242:       0004		RCL C 
0243:       4175		TAD TAI01
0244:       1002		STO A 
0245: M     0000		_ADDI c 66 a
0245:       0000		ZMEM TAI02 66
0245:       0004		RCL C 
0246:       4177		TAD TAI02
0247:       1002		STO A 
0250: M     0000		_ADDI_ c 66 a
0250:       7410		SKP
0251:       0066	IMAD01:	66
0252:       0004		RCL C 
0253:       4251		TAD IMAD01
0254:       1002		STO A 
0255:       0000	
0255:       7402		hlt
0256:       0000	
0256:       0000	; eof

