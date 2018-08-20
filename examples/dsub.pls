0000:       0000	;
0000:       0000	; subtract double words
0000:       0000	;
0000:       0000	
0000:       0765	minend:	0765			; Minuend:	7654321 (oct)
0001:       4321		4321
0002:       0123	subend:	0123			; Subtrahend:	1234567 (oct)
0003:       4567		4567	
0004:       0000	differ:	0			; Differenz:	6417532 (oct)
0005:       0000		0
0006:       0000	
0200:       0000		page
0200:       0000	
0200:       0003	start:	rcl	(subend+1)	; subtrahend, niedriges wort
0201:       7141		cll cma iac		; link leer, 2er komplement
0202:       4001		tad	(minend+1)	; minuend, niedriges wort
0203:       1005		sto	(differ+1)	; ergebnis, niedriger wert
0204:       0002		rcl	subend		; subtrahend, hohes wort
0205:       7040		cma			; 1er komplement bilden
0206:       4000		tad	minend		; minuend, hohes wort + link
0207:       1004		sto 	differ		; ergebnis, hohes wort
0210:       7401		hlt
0211:       0000	

