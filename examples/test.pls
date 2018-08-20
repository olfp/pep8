0000:       0000	;
0000:       0000	; pep8 - opcode tests
0000:       0000	;
0000:       0000	CLLA=	CLA CLL		; Wertsymbol zuweisen
0000:       0000	RET=	JMP I		; Return from Subroutine 
0000:       0000	
0000:       5401		JMP I	VEC	; Erzeugt indirekten Sprung
0001:       0200	VEC:	START		; zum Programmstart
0002:       0000	
0002:       0770	ONE:	0770		; Seicher mit erstem und
0003:       7007	TWO:	7007		; zweitem Wert belegen
0004:       1111	MASK:	1111		; Maske belegen
0005:       7007	ORVAL:	7007		; Oder Wert
0006:       0000	RESULT:	0		; Platz für Ergebnis
0007:       0000	
0007:       0000	
0007:       0000	
0007:       0000	
0200:       0000		PAGE		; Der Code beginnt auf einer 
0200:       0000				; neuen Speicherseite
0200:       0000	
0200:       7300	START:	CLLA		; Wertsymbol referenzieren
0201:       0002		RCL	ONE	; Ersten Wert laden
0202:       4003		TAD	TWO	; Zweiten Wert addieren
0203:       6206		JMS	LOGOP	; Subroutine aufrufen
0204:       1006		STO	RESULT	; Ergebnis speichern
0205:       0000	
0205:       7401		HLT		; Programm anhalten
0206:       0000	
0206:       7000	LOGOP:	NOP		; Platzhalter für Rücksprungadr.
0207:       2004		AND	MASK	; AND mit Maske
0210:       3005		ORA	ORVAL	; OR mit Wert
0211:       5606		RET	LOGOP	; Rücksprung
0212:       0000	
0212:       0000	; eof

