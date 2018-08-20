0000:       0000	; n! (n-Fakultät, also n*(n.1)*(n-2)*...*1) rekursiv berechnen
0000:       0000	
0000:       0400	enter:  gosub		; vektor: start subroutine
0001:       0412	leave:  endsub		; vektor: rücksprung
0002:       0420	acpush:	pushac		; vektor: akku poppen
0003:       0426	acpop:	popac		; vektor: akku pushen
0004:       0000	
0004:       0006	n:	6		; 6! = 720(10) = 1320(8) < 4096
0005:       0000	
0005:       0000	result:	0
0006:       0000	this:	0
0007:       0000	
0200:       0000		page
0200:       0000	
0200:       0004	start:	rcl	n	; damit gehts los
0201:       6400		jsr		; fac(n) berechnen
0202:       0205		fac
0203:       1005		sto	result	; endergebnis
0204:       7401		hlt
0205:       0000	
0205:       7424	fac:	sna		; akku größer null?
0206:       5222		jmp	done	; nein, rekursionsende
0207:       6402		psh		; aktuelles n auf stack
0210:       7041		cma iac		; 2er komplement
0211:       7001		iac 		; erhöhen
0212:       7041		cma iac		; 2er komp. -> ac--
0213:       6400		jsr		; rekursiver aufruf:
0214:       0205		fac		; akku wird fac(n-1)
0215:       1006		sto	this	; sichern
0216:       6403		pop		; aktuelles n holen
0217:       6224		jms	mul	; multiplizieren
0220:       0006		this		; mit zwischenergebnis
0221:       7404		skp		; überspringen, zum rückspr.
0222:       7001	done:	iac		; akku wird eins
0223:       5401		rts		; rücksprung
0224:       0000	
0224:       7000	mul:	nop		; akku = akku * (next)
0225:       1252		sto 	base	; akku sichern, 1. faktor
0226:       0624		rcl i	mul	; wort nach dem aufruf ist 
0227:       1254		sto 	cnt	; zeiger auf zweiten faktor,
0230:       0654		rcl i	cnt	; also dereferenzieren und
0231:       1254		sto 	cnt	; zurückspeichern.
0232:       0224		rcl	mul	; rücksprungaddr
0233:       7001		iac
0234:       1224		sto	mul	; einen weiter
0235:       0000	
0235:       0000		; base * cnt = base + base + ... (cnt)
0235:       0000	
0235:       7300		cla cll		; startwert
0236:       4252	loop:	tad 	base	; einmal mehr
0237:       1253		sto	curr	; zwischenergebnis sichern
0240:       7300		cll cla
0241:       7040		cma		; minus 1
0242:       4254		tad	cnt	; zähler dazu = zähler - 1
0243:       7424		sna		; nicht null? weitermachen!
0244:       5250		jmp	mul0	; fertig
0245:       1254		sto	cnt
0246:       0253		rcl	curr	; zwischenergebnis holen
0247:       5236		jmp	loop	; nächste runde
0250:       0000	
0250:       0253	mul0:	rcl	curr	; zwischen- ist endergebnis
0251:       5624		jmp i	mul	; rücksprung
0252:       0000	
0252:       0000	base:	0
0253:       0000	curr:	0
0254:       0000	cnt:	0
0255:       0000	
0400:       0000		page		
0400:       0000	
0400:       0000	jsr=    jms i	enter	; makro: subroutine anspringen
0400:       0000	rts=    jmp i   leave	; makro: rücksprung aus routine
0400:       0000	psh=	jms i	acpush	; makro: akku auf stack
0400:       0000	pop=	jms i	acpop	; makro: akku vom stack
0400:       0000	
0400:       0000		; subroutinen für subroutinen mit stack
0400:       0000	
0400:       7000	gosub:	nop		; subroutine mit stack aufrufen
0401:       1245		sto	temp	; akku sichern
0402:       0200		rcl	gosub	; zeigt zum wort hinter JSR
0403:       7001		iac		; zieladdresse, überspringen
0404:       1646		sto i	tos	; auf den stack
0405:       6232		jms	tosinc
0406:       0600		rcl i	gosub	; adresse der subroutine
0407:       1200		sto 	gosub	; dereferenzieren
0410:       0245		rcl	temp	; akku zurück
0411:       5600		jmp i	gosub	; subroutine anspringen
0412:       0000	
0412:       0000	endsub:	    		; subroutine mit stack beenden
0412:       1245		sto 	temp	; akku sichern
0413:       6237		jms	tosdec
0414:       0646		rcl i	tos	; rücksprung-vektor
0415:       1200		sto 	gosub	; dereferenzieren
0416:       0245		rcl	temp	; akku zurück
0417:       5600		jmp i	gosub	; rücksprung aus subroutine
0420:       0000	
0420:       0000		; push und pop
0420:       0000	
0420:       7000	pushac:	nop		; akku auf dem stack pushen
0421:       1245		sto	temp	; push soll akku erhalten
0422:       1646		sto i	tos	; push ac
0423:       6232		jms 	tosinc	; stackpointer erhöhen
0424:       0245		rcl	temp	; akku zurückholen
0425:       5620		jmp i	pushac	; rücksprung
0426:       0000	
0426:       7000	popac:	nop 		; akku vom stack poppen
0427:       6237		jms	tosdec	; stackpointer erniedrigen
0430:       0646		rcl i	tos	; akku poppen
0431:       5626		jmp i	popac
0432:       0000	
0432:       0000		; tos manipulieren
0432:       0000	
0432:       7000	tosinc:	nop		; stackpointer erhöhen
0433:       0246		rcl 	tos
0434:       7001		iac		
0435:       1246		sto	tos
0436:       5632		jmp i	tosinc
0437:       0000	
0437:       7000	tosdec:	nop		; stackpointer erniedrigen
0440:       7300		cll cla
0441:       7040		cma		; minus 1
0442:       4246		tad 	tos	; tos dazu
0443:       1246		sto	tos
0444:       5637		jmp i	tosdec
0445:       0000	
0445:       0000	temp:	0
0446:       0000	
0446:       0447	tos:    stack		; pointer to top-of-stack
0447:       0000	stack:	0		; stack, wächst nach oben
