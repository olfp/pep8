0000:       0000	;
0000:       0000	; pep8 - distinguish odd and even
0000:       0000	;
0000:       0000	
0000:       0000	CURR:	0		; laufender wert
0001:       0077	LAST:	77		; maximalwert
0002:       0003	PTR:	ODDS		; zeiger in tabelle
0003:       0000	ODDS:			; ergebnistabelle ab hier
0003:       0000	
0200:       0000		PAGE
0200:       0000	
0200:       7200	START:	CLA		; start wert ist null
0201:       7010	LOOP:	RAR		; bit 11 ins link
0202:       7410		SNL		; link =1, dann ungrade
0203:       5211		JMP 	EVEN	; sonst gerade
0204:       7004		RAL		; ist ungerade, zurück
0205:       1402		STO I	PTR	; abspeichern in tabelle
0206:       0002		RCL 	PTR	; zeiger holen
0207:       7001		IAC		; erhöhen
0210:       1002		STO 	PTR	; und speichern
0211:       0000	EVEN:	RCL	CURR	; aktueller wert
0212:       7141		CLL CMA IAC	; 2er komplement
0213:       4001		TAD	LAST	; last - curr
0214:       7424		SNA		; nicht null, weiter
0215:       5222		JMP	FINIS	; fertig
0216:       0000		RCL	CURR	; aktueller wert
0217:       7001		IAC		; erhöhen
0220:       1000		STO 	CURR	; und speicher
0221:       5201		JMP 	LOOP	; weiter
0222:       7401	FINIS:	HLT
0223:       0000	
0223:       0000	; eof
0223:       0000		
0223:       0000		
0223:       0000		

