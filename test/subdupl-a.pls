0000:       0000	;
0000:       0000	; pep8 -SUBTRAHIEREN DOPPELT GENAUER ZAHLEN
0000:       0000	;
0000:       5401		JMP I 	VEC         
0001:       0200	VEC: 	START           
0002:       0000	
0002:       0064	NOONEH:  64           
0003:       0032	NOONEL:  32           
0004:       0057	NOTWOH:  57 
0005:       0011	NOTWOL:  11 
0006:       0000	NOTREH:  0             
0007:       0000	NOTREL:  0
0010:       0000	
0200:       0000	        PAGE           
0200:       0000	                        
0200:       0000	
0200:       0005	START:  RCL     NOTWOL      ; lädt 23 
0201:       7141	        CMA     IAC    CLL  ; x -1 ,löscht linkbit, invertiert
0202:       4003	        TAD     NOONEL      ; (=-23) +40
0203:       1007	        STO     NOTREL      ; speichert 17
0204:       0004	        RCL	NOTWOH      ; lädt 50
0205:       7040	        CMA	            ; invertieren
0206:       4002	        TAD	NOONEH      ; addiert 40
0207:       1006	        STO     NOTREH      ; speichert 7 (Ergebnis) ????
0210:       7402	FINIS:	HLT

