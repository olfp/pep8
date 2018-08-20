0000:       0000	;
0000:       0000	; pep8 - subtraktion mittels 2er-komplement
0000:       0000	;
0000:       5401		JMP I	VEC	; Erzeugt indirekten Sprung
0001:       0200	VEC:	START		; zum Programmstart
0002:       0000	
0002:       0123	ONE:	123		; Seicher mit erstem und
0003:       0042	TWO:	42		; zweitem Wert belegen
0004:       0000	RESULT:	0		; Platz für Ergebnis (61)
0005:       0000	
0200:       0000		PAGE		; Der Code beginnt auf einer 
0200:       0000				; neuen Speicherseite
0200:       0000	
0200:       0003	START:	RCL	TWO	; Ersten Wert laden
0201:       7041		CMA IAC
0202:       4002		TAD	ONE	; Zweiten Wert addieren
0203:       1004		STO	RESULT	; Ergebnis speichern
0204:       0000	
0204:       7401		HLT		; Programm anhalten
0205:       0000	
0205:       0000	; eof

