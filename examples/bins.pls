0000:       0000	;
0000:       0000	; binäre suche
0000:       0000	;
0000:       0000	
0000:       0042	find:	42
0001:       0000	
0001:       0242	first:	tab
0002:       0265	last:	etab
0003:       7777	mid:	-1
0004:       0000	
0200:       0000		page
0200:       0000	
0200:       0001	start:	rcl	first		; erster tab. wert
0201:       7100		cll 			; link 0
0202:       4002		tad 	last		; letzter + erster
0203:       7110		cll rar			; link 0, rechts rotieren
0204:       1003		sto 	mid		; mid = (last + first)/2
0205:       0403		rcl i	mid		; wert an mid holen
0206:       7141		cll cma iac		; link 0, 2er komplement
0207:       4000		tad 	find		; mit suchwert vergleichen
0210:       7424		sna			; nicht null?
0211:       5231		jmp	found		; doch! fertig
0212:       7440		sma			; kleiner null?
0213:       5221		jmp	right		; nein! rechte hälfte
0214:       7300	left:	cll cla
0215:       7040		cma			; akku -1
0216:       4003		tad	mid		; plus mitte
0217:       1002		sto	last		; neues ende = mid - 1
0220:       5224		jmp	cont		
0221:       0003	right:	rcl	mid		; mitte holen
0222:       7001		iac			; eins weiter ist
0223:       1001		sto 	first		; neuer anfang
0224:       0001	cont:	rcl	first		; anfang holen
0225:       7141		cll cma iac		; link 0, 2er komplement
0226:       4002		tad 	last		; mit ende vergleichen
0227:       7440		sma			; anfang == ende?
0230:       5200		jmp	start		; nein! nächste runde
0231:       0403	found:	rcl i	mid		; mid zeigt auf wert
0232:       7141		cll cma iac		; link 0, 2er komplement
0233:       4000		tad 	find		; mit suchwert vergleichen
0234:       7420		sza			; null wenn gefunden
0235:       5237		jmp	notfnd		; nicht gefunden
0236:       5241		jmp	finis		; und gut
0237:       7200	notfnd:	cla			; null für nicht gefunden
0240:       1003		sto 	mid		; in mid speichern
0241:       7401	finis:	hlt			; und ende
0242:       0000	
0242:       0000	
0242:       0020	tab:	0020
0243:       0021		0021
0244:       0036		0036
0245:       0042		0042
0246:       0067		0067
0247:       0071		0071
0250:       0076		0076
0251:       0104		0104
0252:       0124		0124
0253:       0134		0134
0254:       0170		0170
0255:       0173		0173
0256:       0176		0176
0257:       0212		0212
0260:       0406		0406
0261:       0502		0502
0262:       0532		0532
0263:       0546		0546
0264:       0703		0703
0265:       0706	etab:	0706
