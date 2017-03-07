0000:       0000	; 
0000:       0000	; PEP8 - SUBTRAHIEREN                        
0000:       0000	;
0000:       0000	CLLA=   CLA CLL                              
0000:       0000	  
0000:       0000	  
0000:       0200	VEC:    START
0001:       0000	
0001:       0200	NOONEL:  200
0002:       0023	NOTWOL:  23
0003:       0064	NOONEH:  64
0004:       0017	NOTWOH:  17
0005:       0007	NOTHRE:  7
0006:       0000	RESULT: 0
0007:       0000	
0200:       0000	        PAGE
0200:       0000	  
0200:       0000	
0200:       7300	START:  CLLA
0201:       0001	        RCL     NOONEL
0202:       7040	        CMA
0203:       4002	        TAD     NOTWOL
0204:       1005	        STO     NOTHRE
0205:       0004	        RCL     NOTWOH
0206:       7040	        CMA
0207:       4003	        TAD     NOONEH
0210:       1006	        STO     RESULT
0211:       7402	        HLT

