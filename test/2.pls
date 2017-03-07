0000:       0000	; 
0000:       0000	; PEP8 - SUBTRAHIEREN                        
0000:       0000	; 
0000:       0000	CLLA=   CLA CLL
0000:       0000	  
0000:       0000	  
0000:       0200	VEC:    START
0001:       0000	
0001:       0001	ONE:  1
0002:       0023	TWO:  23
0003:       0000	RESULT: 0
0004:       0000	
0200:       0000	        PAGE
0200:       0000	
0200:       0000	
0200:       7300	START:  CLLA
0201:       4001	        TAD     ONE
0202:       0001	        RCL     ONE
0203:       7141	        CMA     IAC CLL
0204:       4002	        TAD     TWO
0205:       7040	        CMA
0206:       1003	        STO     RESULT
0207:       7402	        HLT

