0000:       0000	;
0000:       0000	; Aufddieren der Werte einer Tabelle
0000:       0000	;
0000:       0000	
0000:       0000	CLLA=	CLA CLL		; Wertsymbol zuweisen
0000:       0000	
0000:       5401		JMP I	VEC	; Erzeugt indirekten Sprung
0001:       0200	VEC:	START		; zum Programmstart
0002:       0000	
0002:       0000	RESULT:	0		; Platz für das Ergebnis
0003:       0000	
0003:       0001	TAB:	1		; Die 
0004:       0003		3		; ersten
0005:       0005		5		; sechs
0006:       0007		7		; Primzahlen.
0007:       0013		13		; Natürlich
0010:       0015	ENDT:	15		; oktal.
0011:       0000	
0011:       0006	TLEN:	(ENDT-TAB+1)	; Länge der Tabelle
0012:       0000	
0012:       0003	TPTR:	TAB		; Zeiger zum Anfang
0013:       0010	TEND:	ENDT		; und Ende der Tabelle
0014:       0000	
0200:       0000		PAGE		; Der Code beginnt auf
0200:       0000				; einer neuen Speicherseite
0200:       0000	
0200:       0002	START:	RCL	RESULT	; Zwischenergebiss holen
0201:       7100		CLL		; Link löschen
0202:       4412		TAD I	TPTR	; Nächsten Wert addieren
0203:       1002		STO	RESULT	; Neues Zwischenergebniss
0204:       0012		RCL	TPTR	; Zeiger holen
0205:       7141		CLL CMA IAC	; Zum vergleichen subtrahieren
0206:       4013		TAD	TEND	; Zeiger mit Endezeiger vergleichen 
0207:       7424		SNA		; Nicht gleich? Weitermachen
0210:       5215		JMP	FINISH	; Sonst fertig
0211:       0012		RCL	TPTR	; Zeiger holen
0212:       7001		IAC		; erhöhen
0213:       1012		STO	TPTR	; und zurückspeichern
0214:       5200		JMP	START	; Nächster Wert
0215:       7401	FINISH:	HLT

