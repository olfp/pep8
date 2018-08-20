0000:       0000	;
0000:       0000	; pep8 - addieren von 1 und 2
0000:       0000	;
0000:       0000	CLLA=	CLA CLL		; Wertsymbol zuweisen
0000:       0000	
0000:       5401		JMP I	VEC	; Erzeugt indirekten Sprung
0001:       0200	VEC:	START		; zum Programmstart
0002:       0000	
0002:       0001	ONE:	1		; Seicher mit erstem und
0003:       0002	TWO:	2		; zweitem Wert belegen
0004:       0000	RESULT:	0		; Platz für Ergebnis
0005:       0000	
0200:       0000		PAGE		; Der Code beginnt auf einer 
0200:       0000				; neuen Speicherseite
0200:       0000	
0200:       7300	START:	CLLA		; Wertsymbol referenzieren
0201:       0002		RCL	ONE	; Ersten Wert laden
0202:       4003		TAD	TWO	; Zweiten Wert addieren
0203:       1004		STO	RESULT	; Ergebnis speichern
0204:       0000	
0204:       7401		HLT		; Programm anhalten
0205:       0000	
0205:       0000	; eof

