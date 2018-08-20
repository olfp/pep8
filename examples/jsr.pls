0000:       0000	;
0000:       0000	; emulate return stack on pep8
0000:       0000	;
0000:       0000	
0000:       5401		JMP I	VEC
0001:       0200	VEC:	START
0002:       0000	
0002:       0400	ENTER:	GOSUB			; vector: start subroutine
0003:       0412	LEAVE:	ENDSUB			; vector: return from subroutine
0004:       0000	
0004:       0000	PTR:	0			; helper pointer
0005:       0600	TOS:	STACK			; pointer to top-of-stack
0006:       0000	
0006:       0010	CNT:	10			; variables
0007:       0000	
0200:       0000		PAGE
0200:       0000	
0200:       6402	START:	JSR			; call
0201:       0203		CNTDN			; countdown subroutine
0202:       7401		HLT			; cnt should be 0
0203:       0000	
0203:       7300	CNTDN:	CLL CLA			; countdown subroutine
0204:       7040		CMA
0205:       4006		TAD	CNT		; decrement
0206:       1006		STO	CNT
0207:       7424		SNA			; ac non-zero, continue
0210:       5213		JMP 	FINIS		; ac was zero, terminate
0211:       6402		JSR
0212:       0203		CNTDN			; recursion
0213:       5403	FINIS:	RET
0214:       0000	
0400:       0000		PAGE			; stack calls need to work cross-page
0400:       0000	
0400:       7000	GOSUB:	NOP
0401:       0200		RCL	GOSUB		; points to word after JSR
0402:       7001		IAC			; occupied with target addr, skip
0403:       1405		STO I	TOS		; put on stack
0404:       0000	
0404:       0005		RCL 	TOS
0405:       7001		IAC			; increment top-of-stack
0406:       1005		STO	TOS
0407:       0000	
0407:       0600		RCL I	GOSUB		; address of subroutine
0410:       1004		STO 	PTR		; dereference
0411:       5404		JMP I	PTR		; jump to subroutine
0412:       0000	
0412:       7300	ENDSUB:	CLL CLA
0413:       7040		CMA
0414:       4005		TAD 	TOS		; decrement top-of-stack
0415:       1005		STO	TOS
0416:       0405		RCL I	TOS		; return vector
0417:       1004		STO 	PTR		; dereference
0420:       5404		JMP I	PTR		; return from subroutine
0421:       0000	
0421:       0000	JSR=	JMS I	ENTER		; macro: enter subroutine
0421:       0000	RET=	JMP I	LEAVE		; macro: return from subroutine
0421:       0000	
0600:       0000		PAGE
0600:       0000	
0600:       0000	STACK:				; 128 word stack growing upwards
0600:       0000	

