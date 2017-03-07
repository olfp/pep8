0000:       0000	; 
0000:       0000	; PEP8 - SUBTRAHIEREN
0000:       0000	; 
0000:       0000	CLLA=   CLA CLL                              
0000:       0000	  
0000:       5401	  	JMP I	VEC
0001:       0200	VEC:    START
0002:       0000	
0002:       0034	NOONEL: 34
0003:       0056	NOTWOL: 56
0004:       0000	NOONEH:	0
0005:       0000	NOTWOH: 0
0006:       0000	NOTHRE: 0
0007:       0000	RESULT: 0
0010:       0000	
0200:       0000	        PAGE
0200:       0000	  
0200:       0000	
0200:       7300	START:  CLLA
0201:       0003	        RCL     NOTWOL
0202:       7141	        CMA     IAC    CLL
0203:       4002	        TAD     NOONEL
0204:       1006	        STO     NOTHRE
0205:       0005	        RCL     NOTWOH
0206:       7040	        CMA
0207:       4004	        TAD     NOONEH
0210:       1007	        STO     RESULT
0211:       7402	        HLT

