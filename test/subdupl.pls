0000:       0000	;
0000:       0000	; PEP8 -SUBTRAHIEREN DOPPELT GENAUER ZAHLEN
0000:       0000	;
0000:       0000	CLLA=   CLA CLL        
0000:       5401	JMP I VEC         
0001:       0200	VEC: START           
0002:       0000	
0002:       0040	NOONEH:  40           
0003:       0013	NOONEL:  13           
0004:       0023	NOTWOH:  23 
0005:       0050	NOTWOL:  50
0006:       0000	NOTREH:  0             
0007:       0000	NOTREL:  0
0010:       0000	
0200:       0000	        PAGE           
0200:       0000	                        
0200:       0000	
0200:       7300	START:  CLLA    
0201:       0004	        RCL     NOTWOH      ; L‰DT 23 
0202:       7141	        CMA     IAC    CLL  ; X -1 ,LˆSCHT LINKBIT, INVERTIERT
0203:       4002	        TAD     NOONEH      ; (=-23) +40
0204:       1006	        STO     NOTREH      ; SPEICHERT 17
0205:       0005	        RCL	NOTWOL      ; L‰DT 50
0206:       7040	        CMA	            ; INVERTIEREN
0207:       4002	        TAD	NOONEH      ; ADDIERT 40
0210:       1007	        STO     NOTREL      ; SPEICHERT 7 (ERGEBNIS) ????
0211:       7402		HLT

