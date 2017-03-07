0000:       0000	; 
0000:       0000	; PEP8 - SUBTRAHIEREN                        
0000:       0000	; 
0000:       0000	CLLA=   CLA CLL                              
0000:       0000	  
0000:       0000	
0000:       0200	VEC:    START
0001:       0000	
0001:       0007	ONE:  7
0002:       0005	TWO:  5
0003:       0000	RESULT: 0
0004:       0000	
0200:       0000	        PAGE
0200:       0000	
0200:       0000	
0200:       7300	START: CLLA
0201:       0002	       RCL TWO
0202:       7141	       CMA IAC CLL
0203:       4001	       TAD ONE
0204:       1003	       STO RESULT
0205:       7402	       HLT

