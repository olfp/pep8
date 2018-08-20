0000:       0000	;
0000:       0000	; bubblesort
0000:       0000	;
0000:       0000	
0000:       0000	begin=	7777		; sub start marker
0000:       0000	end=	jmp i		; sub return 
0000:       0000	
0000:       0000	ptr1:	0		; zeiger auf erstes datenelement
0001:       0000	ptr2:	0		; zeiger auf zweites datenelement
0002:       0000	
0002:       0400	tabsp:	tab		; zeiger auf tabellenstart
0003:       0423	tabep:	etab		; zeiger auf tabellenende
0004:       0024	tablen:	(etab-tab+1)	; länge datentabelle
0005:       0000	
0005:       0000	xcnt:	0		; anzahl vertauschungen pro lauf
0006:       0000	
0006:       0000	temp:	0		; vorrübergehendes speicherwort
0007:       0000	
0200:       0000		page
0200:       0000	
0200:       7200	start:	cla		; äußere schleife
0201:       1005		sto	xcnt	; vertauschungszähler initialisieren
0202:       0002		rcl	tabsp	; tabellenstart holen
0203:       1000		sto	ptr1	; ersten zeiger initialisieren
0204:       7001		iac		; eine  weiterschieben
0205:       1001		sto	ptr2	; zweiten zeiger initialisieren
0206:       0000	
0206:       0401	loop:	rcl i	ptr2	; zweites datenelement holen
0207:       7141		cll cma iac 	; link löschen, zweierkomplement bilden
0210:       4400		tad i	ptr1	; erstes datenelement addieren
0211:       7440		sma 		; zweites größer erstes? dann nix tun
0212:       6231		jms	exch	; sonst vertauschen
0213:       0001		rcl	ptr2	; zweiten zeiger holen
0214:       7141		cll cma iac	; link löschen, zweierkomplement bilden
0215:       4003		tad 	tabep	; tabellenende addieren
0216:       7424		sna		; ungleich null, zweiter < ende
0217:       5225		jmp	chkcnt	; sonst ende duchlauf, abbruch prüfen
0220:       0001		rcl	ptr2	; zweiten zeiger laden
0221:       1000		sto	ptr1	; als ersten ablegen
0222:       7001		iac		; weiterschieben
0223:       1001		sto	ptr2	; als neuen zweiten ablegen
0224:       5206		jmp	loop	; und vergleichen
0225:       0000	
0225:       0005	chkcnt:	rcl	xcnt	; anzahl der vertauschungen
0226:       7420		sza		; wenn  null, fertig
0227:       5200		jmp	start	; sonst weitermachen
0230:       7401	finis:	hlt
0231:       0000	
0231:       7777	exch:	begin		; datenworte an ptr1 und ptr2 tauschen
0232:       0400		rcl i	ptr1	; erstes wort laden
0233:       1006		sto 	temp	; aufheben
0234:       0401		rcl i	ptr2	; zweites wort laden
0235:       1400		sto i	ptr1	; als erstes sichen
0236:       0006		rcl 	temp	; gesicherte wort laden
0237:       1401		sto i	ptr2	; als zweites sichern
0240:       0005		rcl 	xcnt	; vertauschungszähler holen
0241:       7001		iac		; erhöhen
0242:       1005		sto	xcnt	; und sichern
0243:       5631		end	exch	; rücksprung
0244:       0000	
0400:       0000		page
0400:       0000	
0400:       0124	tab:	124		; datentabelle
0401:       0706		706
0402:       0076		076
0403:       0067		067
0404:       0021		021
0405:       0036		036
0406:       0170		170
0407:       0502		502
0410:       0020		020
0411:       0406		406
0412:       0173		173
0413:       0104		104
0414:       0042		042
0415:       0212		212
0416:       0176		176
0417:       0546		546
0420:       0071		071
0421:       0703		703
0422:       0134		134
0423:       0532	etab:	532		; ende daten

