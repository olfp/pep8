0000:       0000	;
0000:       0000	; simple rpn calculator
0000:       0000	;
0000:       0000	
0000:       0000	
0000:       5401		JMP I	VEC		; jumpstart (just in case)
0001:       0200	VEC:	START
0002:       0000	
0002:       0400	WORDS:	DOWRDS			; longjump table
0003:       0723	INCTOS:	INTOS
0004:       0730	DECTOS:	DETOS
0005:       0652	PUTSV:	PUTS
0006:       0600	DNUMPR:	DECNUM
0007:       0000	
0007:       0000	SAR=	CLL RAR			; shift ac right
0007:       0000	CIAL=	CLL CMA IAC		; 2's complement, link clear
0007:       0000	PRINT=	JMS I	PUTSV		; print following counted str
0007:       0000	
0007:       0000	TTY=	0			; TTY is device zero
0007:       0000	
0007:       1427	LINEP:	LINBUF			; pointer to line buffer
0010:       1427	CHARP:	LINBUF			; pointer to current char
0011:       1576	ENDP:	(ENDBUF-2)		; pointer behind buffer (one for \0)
0012:       0000	
0012:       1600	BOS:	STACK			; base of stack
0013:       1600	TOS:	STACK			; top of stack
0014:       0000	
0014:       0004	EOF:	4			; ^D, end of file
0015:       0012	NL:	12			; ^J, new line
0016:       0015	CR:	15			; ^M, carriage retuen
0017:       0040	SPACE:	40			; ' '
0020:       0060	CHAR0:	60			; 0
0021:       0071	CHAR9:	71			; 9
0022:       0000	
0022:       0077	ERRCH:	77			; ?
0023:       0041	OKCHAR:	41			; !
0024:       0000	
0024:       0000	NUMVAL:	0
0025:       0000	NUMFLG:	0
0026:       0000	
0026:       0000	TEMP:	0
0027:       0000	CNT:	0
0030:       0000	
0030:       1400	FUNB:	FUNTAB			; func pointer base
0031:       1400	FUNP:	FUNTAB			; current func pointer
0032:       0000	
0032:       7000	GETCH:	NOP			; get char from tty
0033:       7600	WAITI:	SRI	TTY
0034:       5033		JMP	WAITI
0035:       7640		DGA	TTY		; read char
0036:       5432		JMP I	GETCH
0037:       0000	
0037:       7000	PUTCH:	NOP			; print char on tty
0040:       7620	WAITO:	SRO	TTY
0041:       5040		JMP	WAITO
0042:       7660		DPA	TTY		; print char
0043:       5437		JMP I	PUTCH
0044:       0000	
0044:       7000	TERPRI:	NOP			; print cr-lf
0045:       0016	  	RCL	CR		
0046:       6037		JMS	PUTCH
0047:       0015		RCL	NL
0050:       6037		JMS	PUTCH
0051:       5444		JMP I	TERPRI
0052:       0000	
0200:       0000		PAGE
0200:       0000	
0200:       6213	START:	JMS	BANNER		; print startup banner
0201:       6234	MNLOOP:	JMS	READLN		; read line from tty
0202:       0407		RCL I	LINEP		; get first char
0203:       7141		CIAL
0204:       4014		TAD	EOF		; compare with eof
0205:       7424		SNA
0206:       5212		JMP	FINIS		; got eof
0207:       6402		JMS I	WORDS		; analyse line
0210:       6044		JMS	TERPRI
0211:       5201		JMP	MNLOOP		; loop
0212:       0000	
0212:       7401	FINIS:	HLT
0213:       0000	
0213:       7000	BANNER:	NOP
0214:       6044		JMS	TERPRI
0215:       6405		PRINT
0216:       0025		TEXT	/RPN V1.1 (c) olf 2005/
0217:       6260
0220:       5600
0221:       6621
0222:       1621
0223:       0010
0224:       4311
0225:       0057
0226:       5446
0227:       0022
0230:       2020
0231:       2517
0232:       6044		JMS	TERPRI
0233:       5613		JMP I	BANNER
0234:       0000	
0234:       7000	READLN:	NOP			; read line from tty with echo
0235:       0007		RCL	LINEP
0236:       1010		STO	CHARP		; reset char pointer
0237:       7200		CLA
0240:       1410		STO I	CHARP		; init end of line
0241:       6032	CHLOOP:	JMS	GETCH		; read char
0242:       1410		STO I	CHARP		; save to buffer
0243:       6037		JMS 	PUTCH		; echo char	
0244:       0000	
0244:       0010		RCL 	CHARP
0245:       7141		CIAL
0246:       4011		TAD	ENDP		; compare with end of buffer
0247:       7424		SNA
0250:       5276		JMP	OVERFL		; end buffer, return
0251:       0410		RCL I	CHARP
0252:       7141		CIAL
0253:       4014		TAD	EOF		; compare to eof
0254:       7424		SNA
0255:       5307		JMP	ENDLN		; return if eof
0256:       0410		RCL I	CHARP
0257:       7141		CIAL
0260:       4015		TAD	NL		; compare to nl
0261:       7424		SNA
0262:       5300		JMP	ADDCR		; print cr, return
0263:       0410		RCL I	CHARP
0264:       7141		CIAL
0265:       4016		TAD	CR		; compare to cr
0266:       7424		SNA
0267:       5303		JMP	ADDNL		; print nl, return
0270:       0010		RCL 	CHARP
0271:       7001		IAC
0272:       1010		STO	CHARP		; advance char pointer
0273:       7200		CLA
0274:       1410		STO I	CHARP		; mark end
0275:       5241		JMP	CHLOOP		; next char
0276:       0000	
0276:       6044	OVERFL:	JMS	TERPRI		; buffer overflow, terminate line
0277:       5307		JMP	ENDLN
0300:       0000	
0300:       0016	ADDCR:	RCL	CR
0301:       6037		JMS	PUTCH
0302:       5307		JMP	ENDLN
0303:       0000	
0303:       0015	ADDNL:	RCL	NL
0304:       1410		STO I	CHARP		; convert cr to nl in line
0305:       6037		JMS	PUTCH
0306:       5307		JMP	ENDLN
0307:       0000	
0307:       0010	ENDLN:	RCL 	CHARP
0310:       7001		IAC
0311:       1010		STO	CHARP		; advance char pointer
0312:       7200		CLA
0313:       1410		STO I	CHARP		; mark end of line
0314:       0000	
0314:       5634		JMP I	READLN		; return to main loop
0315:       0000	
0400:       0000		PAGE
0400:       0000		
0400:       7000	DOWRDS:	NOP			; analyse line
0401:       0007		RCL	LINEP
0402:       1010		STO	CHARP
0403:       0410	SKIPSP:	RCL I	CHARP
0404:       7141		CIAL
0405:       4017		TAD	SPACE
0406:       7420		SZA
0407:       5214		JMP	WRLOOP
0410:       0010		RCL	CHARP
0411:       7001		IAC
0412:       1010		STO	CHARP
0413:       5203		JMP	SKIPSP
0414:       0410	WRLOOP:	RCL I	CHARP
0415:       7424		SNA
0416:       5340		JMP	TERMCH		; end of line
0417:       7141		CIAL
0420:       4015		TAD	NL		; compare to nl
0421:       7424		SNA
0422:       5320		JMP	NEXTW		; next (last) word
0423:       0410		RCL I	CHARP
0424:       7141		CIAL
0425:       4017		TAD	SPACE		; compare to ' '
0426:       7424		SNA
0427:       5320		JMP	NEXTW		; next word
0430:       0410		RCL I	CHARP
0431:       7141		CIAL
0432:       4020		TAD	CHAR0		; compare to '0'
0433:       7460		SMA SZA			; inverted cond
0434:       5262		JMP	NODIG		; not a digit
0435:       0410		RCL I	CHARP
0436:       7141		CIAL
0437:       4021		TAD	CHAR9		; compare to '9'
0440:       7444		SPA			; inverted cond
0441:       5262		JMP	NODIG		; not a digit
0442:       0000		
0442:       6341		JMS	MUL10		; neue stelle
0443:       0020		RCL	CHAR0
0444:       7141		CIAL
0445:       4410		TAD I	CHARP		; wert bilden
0446:       7100		CLL
0447:       4024		TAD	NUMVAL		; dazu
0450:       1024		STO	NUMVAL
0451:       7201		CLA IAC
0452:       1025		STO	NUMFLG
0453:       7100		CLL			; kein wortende
0454:       0000	
0454:       0010	NXTCHR:	RCL	CHARP		; advance char pointer
0455:       7001		IAC
0456:       1010		STO	CHARP
0457:       7410		SNL
0460:       5214		JMP	WRLOOP
0461:       5203		JMP	SKIPSP
0462:       0000	
0462:       0025	NODIG:	RCL	NUMFLG		; already seen some digits?
0463:       7424		SNA
0464:       5272		JMP	FINCMD
0465:       0010		RCL	CHARP
0466:       7141		CIAL			; no DAC so CIAL CMA to decrement
0467:       7040		CMA			; this way the non-digit is seen again
0470:       1010		STO	CHARP		; and processed as a cmd char
0471:       5320		JMP	NEXTW
0472:       6354	FINCMD:	JMS	FINDC
0473:       7420		SZA
0474:       5335		JMP	EXEC
0475:       0022		RCL	ERRCH
0476:       6037		JMS	PUTCH	
0477:       6044		JMS	TERPRI
0500:       0000	
0500:       0010	NEXTSP:	RCL	CHARP	
0501:       7001		IAC
0502:       1010		STO	CHARP
0503:       0410		RCL I	CHARP
0504:       7424		SNA
0505:       5340		JMP	TERMCH
0506:       7141		CIAL
0507:       4015		TAD	NL
0510:       7424		SNA
0511:       5340		JMP	TERMCH
0512:       0410		RCL I	CHARP
0513:       7141		CIAL
0514:       4017		TAD	SPACE
0515:       7420		SZA
0516:       5300		JMP	NEXTSP
0517:       5254		JMP	NXTCHR
0520:       0000	
0520:       0025	NEXTW:	RCL	NUMFLG
0521:       7424		SNA
0522:       5334		JMP	NONUMB
0523:       0024		RCL	NUMVAL
0524:       1413		STO I	TOS
0525:       0013		RCL	TOS
0526:       7001		IAC
0527:       1013		STO	TOS
0530:       7200		CLA
0531:       1024		STO	NUMVAL
0532:       1025		STO	NUMFLG
0533:       7120		STL			; flag wortende
0534:       5254	NONUMB:	JMP	NXTCHR
0535:       0000	
0535:       1001	EXEC:	STO	VEC
0536:       6401		JMS I	VEC
0537:       5300		JMP	NEXTSP
0540:       0000	
0540:       6600	TERMCH:	JMS I	DOWRDS
0541:       0000	
0541:       7000	MUL10:	NOP			; numval *= 10
0542:       0024		RCL	NUMVAL
0543:       7104		CLL RAL
0544:       7104		CLL RAL
0545:       7104		CLL RAL			; mal acht
0546:       7100		CLL
0547:       4024		TAD	NUMVAL
0550:       7100		CLL
0551:       4024		TAD	NUMVAL
0552:       1024		STO	NUMVAL
0553:       5741		JMP I	MUL10
0554:       0000	
0554:       7000	FINDC:	NOP
0555:       0030		RCL	FUNB
0556:       1031	FLOOP:	STO	FUNP
0557:       0431		RCL I	FUNP		; get cmd char
0560:       7424		SNA
0561:       5376		JMP	FDONE		; nothing found
0562:       7141		CIAL
0563:       4410		TAD I	CHARP		; compare
0564:       7424		SNA
0565:       5372		JMP	FUFOU
0566:       0031		RCL	FUNP
0567:       7001		IAC
0570:       7001		IAC			; advance pointer
0571:       5356		JMP	FLOOP		; check next
0572:       0031	FUFOU:	RCL	FUNP
0573:       7001		IAC
0574:       1031		STO	FUNP		; position to vector
0575:       0431		RCL I	FUNP		; return vec
0576:       5754	FDONE:	JMP I	FINDC
0577:       0000		
0600:       0000		PAGE
0600:       0000	
0600:       7000	DECNUM:	NOP			; print tos as decimal
0601:       0413		RCL I	TOS
0602:       1251		STO	NUMBER		; put in work area
0603:       0246		RCL	DECMAX
0604:       1247		STO	DECP		; current place
0605:       0647	NXTSUB:	RCL I	DECP
0606:       7141		CIAL
0607:       4251		TAD	NUMBER
0610:       7444		SPA
0611:       5217		JMP	NXTDIG		; underflow, next digit
0612:       1251		STO	NUMBER
0613:       0250		RCL	DIGIT
0614:       7001		IAC
0615:       1250		STO	DIGIT		; increment digit
0616:       5205		JMP	NXTSUB
0617:       0000	
0617:       0250	NXTDIG:	RCL	DIGIT
0620:       7424		SNA	
0621:       5226		JMP	ISZERO
0622:       7100		CLL
0623:       4020		TAD	CHAR0
0624:       6037		JMS	PUTCH
0625:       7200		CLA
0626:       1250	ISZERO:	STO	DIGIT
0627:       0247		RCL	DECP
0630:       7001		IAC
0631:       1247		STO	DECP
0632:       0647		RCL I	DECP
0633:       7420		SZA
0634:       5205		JMP	NXTSUB
0635:       0000	
0635:       0251		RCL	NUMBER		; do last digit
0636:       7100		CLL
0637:       4020		TAD	CHAR0
0640:       6037		JMS	PUTCH
0641:       0000		
0641:       5600		JMP I	DECNUM
0642:       0000	
0642:       1750	DECTAB:	1750			; 1000 dec
0643:       0144		144			; 100 dec
0644:       0012		12			; 10 dec
0645:       0000		0
0646:       0000	
0646:       0642	DECMAX:	DECTAB
0647:       0000	DECP:	0
0650:       0000	DIGIT:	0
0651:       0000	NUMBER: 0
0652:       0000	
0652:       7000	PUTS:	NOP			; print following counted str
0653:       0652		RCL I	PUTS		; ret vec is ptr
0654:       1320		STO 	LEN		; first word is length
0655:       0252	PLOOP:	RCL	PUTS
0656:       7001		IAC
0657:       1252		STO	PUTS		; increment to first char(s)
0660:       7100		CLL			; unflag inword
0661:       6310		JMS	CHKLEN
0662:       0652		RCL I	PUTS		
0663:       7110		SAR
0664:       7110		SAR
0665:       7110		SAR
0666:       7110		SAR
0667:       7110		SAR
0670:       7110		SAR
0671:       7100		CLL
0672:       4321		TAD	ASCII
0673:       6037		JMS	PUTCH
0674:       7120		STL			; flag inword
0675:       6310		JMS	CHKLEN
0676:       0652		RCL I	PUTS		; lower sixbit
0677:       2322		AND	MASK6
0700:       7100		CLL
0701:       4321		TAD	ASCII
0702:       6037		JMS	PUTCH
0703:       5255		JMP	PLOOP
0704:       7200	PDONE:	CLA
0705:       4252		TAD	PUTS		; add inword flag
0706:       1252		STO	PUTS
0707:       5652		JMP I	PUTS
0710:       0000	
0710:       7000	CHKLEN:	NOP
0711:       0320		RCL	LEN
0712:       7424		SNA
0713:       5304		JMP	PDONE
0714:       7041		CMA IAC
0715:       7040		CMA
0716:       1320		STO	LEN
0717:       5710		JMP I	CHKLEN
0720:       0000	
0720:       0000	LEN:	0			; length countdown
0721:       0040	ASCII:	40			; offset sixbit acsii
0722:       0077	MASK6:	77			; sixbit mask
0723:       0000	
0723:       7000	INTOS:	NOP			; tos++
0724:       0013		RCL	TOS
0725:       7001		IAC
0726:       1013		STO	TOS
0727:       5723		JMP I	INTOS
0730:       0000	
0730:       7000	DETOS:	NOP			; tos--
0731:       0013		RCL	TOS
0732:       7141		CIAL
0733:       4012		TAD	BOS		; underflow?
0734:       7440		SMA
0735:       5343		JMP	UFLOW		; yep
0736:       7300		CLL CLA 
0737:       7040		CMA			; load -1
0740:       4013		TAD	TOS
0741:       1013		STO	TOS		; tos+-1
0742:       5353		JMP	TOSSED
0743:       6405	UFLOW:	PRINT
0744:       0012		TEXT	/?UNDERFLOW/
0745:       3765
0746:       5644
0747:       4562
0750:       4654
0751:       5767
0752:       7200		CLA			; error: return zero
0753:       5730	TOSSED:	JMP I	DETOS
0754:       0000		
1000:       0000		PAGE
1000:       0000	
1000:       7000	FQUIT:	NOP			; quit
1001:       6044		JMS	TERPRI
1002:       7401		HLT
1003:       0000	
1003:       7000	FHELP:	NOP			; print help
1004:       6405		PRINT
1005:       0115		TEXT	#ARTITH: + - * / PRINT TOS: . DROP TOS: , CLEAR STACK: C SHOW STACK: S QUIT: Q#
1006:       4162
1007:       6451
1010:       6450
1011:       3200
1012:       1300
1013:       1500
1014:       1200
1015:       1700
1016:       6062
1017:       5156
1020:       6400
1021:       6457
1022:       6332
1023:       0016
1024:       0044
1025:       6257
1026:       6000
1027:       6457
1030:       6332
1031:       0014
1032:       0043
1033:       5445
1034:       4162
1035:       0063
1036:       6441
1037:       4353
1040:       3200
1041:       4300
1042:       6350
1043:       5767
1044:       0063
1045:       6441
1046:       4353
1047:       3200
1050:       6300
1051:       6165
1052:       5164
1053:       3200
1054:       6103
1055:       6044		JMS	TERPRI
1056:       5603		JMP I	FHELP
1057:       0000	
1057:       7000	FCLEAR:	NOP			; clear stack
1060:       0012		RCL	BOS
1061:       1013		STO	TOS
1062:       5657		JMP I	FCLEAR
1063:       0000	
1063:       7000	FDROP:	NOP			; drop tos
1064:       6404		JMS I	DECTOS
1065:       5663		JMP I	FDROP
1066:       0000	
1066:       7000	FSHOW:	NOP			; show stack
1067:       0013		RCL	TOS
1070:       1026		STO	TEMP
1071:       0012		RCL	BOS
1072:       7141		CIAL
1073:       4026		TAD	TEMP
1074:       7424		SNA
1075:       5310		JMP	ESTACK
1076:       6404	SLOOP:	JMS I	DECTOS
1077:       6406		JMS I	DNUMPR		; print decimal
1100:       0017		RCL	SPACE
1101:       6037		JMS	PUTCH	
1102:       0013		RCL	TOS
1103:       7141		CIAL
1104:       4012		TAD	BOS
1105:       7420		SZA
1106:       5276		JMP	SLOOP
1107:       5320		JMP	ESHOW
1110:       6405	ESTACK:	PRINT
1111:       0013		TEXT	/STACK EMPTY/
1112:       6364
1113:       4143
1114:       5300
1115:       4555
1116:       6064
1117:       7117
1120:       6044	ESHOW:	JMS	TERPRI
1121:       0026		RCL	TEMP
1122:       1013		STO	TOS
1123:       5666		JMP I	FSHOW
1124:       0000	
1124:       7000	FPRINT:	NOP			; print tos
1125:       6404		JMS I 	DECTOS		; decrement tos
1126:       7420		SZA
1127:       6406		JMS I	DNUMPR		; print decimal
1130:       6044		JMS	TERPRI		; cr lf
1131:       6403		JMS I	INCTOS		; re-adjust tos
1132:       5724		JMP I	FPRINT
1133:       0000	
1133:       7000	FADD:	NOP			; add tos, nos
1134:       6404		JMS I	DECTOS
1135:       0413		RCL I	TOS		; first value
1136:       1026		STO	TEMP
1137:       6404		JMS I	DECTOS
1140:       0413		RCL I	TOS		; second value
1141:       7100		CLL
1142:       4026		TAD	TEMP		; do actual add
1143:       1413		STO I	TOS
1144:       6403		JMS I	INCTOS
1145:       5733		JMP I	FADD
1146:       0000	
1146:       7000	FSUB:	NOP			; sub tos from nos
1147:       6404		JMS I	DECTOS
1150:       0413		RCL I	TOS		; value to sub
1151:       7041		CMA IAC			; complement
1152:       1026		STO	TEMP
1153:       6404		JMS I	DECTOS
1154:       0413		RCL I	TOS		; value to sub from
1155:       7100		CLL
1156:       4026		TAD	TEMP		; subtract
1157:       1413		STO I	TOS
1160:       6403		JMS I	INCTOS
1161:       5746		JMP I	FSUB
1162:       0000	
1200:       0000		PAGE
1200:       0000	
1200:       7000	FMUL:	NOP			; multiply tos and nos
1201:       7200		CLA
1202:       1026		STO	TEMP		; start at zero
1203:       6404		JMS I	DECTOS
1204:       0413		RCL I	TOS		; first value 
1205:       1027		STO	CNT
1206:       6404		JMS I	DECTOS
1207:       0026	MLOOP:	RCL	TEMP
1210:       7100		CLL
1211:       4413		TAD I	TOS		; add second value
1212:       1026		STO	TEMP
1213:       7300		CLL CLA
1214:       7040		CMA
1215:       4027		TAD	CNT		; decrement cnt
1216:       1027		STO	CNT
1217:       7420		SZA			; cnt zero, done
1220:       5207		JMP	MLOOP
1221:       0026		RCL	TEMP
1222:       1413		STO I	TOS
1223:       6403		JMS I	INCTOS
1224:       5600		JMP I	FMUL
1225:       0000	
1225:       7000	FDIV:	NOP			; divide nos by tos
1226:       7200		CLA
1227:       1027		STO	CNT		; start at zero
1230:       6404		JMS I	DECTOS
1231:       0413		RCL I	TOS
1232:       7041		CMA IAC
1233:       1026		STO	TEMP		; complement of divisor
1234:       6404		JMS I	DECTOS
1235:       0026	DLOOP:	RCL	TEMP
1236:       7100		CLL
1237:       4413		TAD I	TOS		; subtract
1240:       7444		SPA
1241:       5247		JMP	DDONE		; less than zero, done
1242:       1413		STO I 	TOS
1243:       0027		RCL	CNT
1244:       7001		IAC
1245:       1027		STO	CNT
1246:       5235		JMP	DLOOP
1247:       0027	DDONE:	RCL	CNT
1250:       1413		STO I	TOS
1251:       6403		JMS I	INCTOS
1252:       5625		JMP I	FDIV
1253:       5625		JMP I	FDIV
1254:       0000		
1400:       0000		PAGE
1400:       0000	
1400:       0123	FUNTAB:	123			; 'S'
1401:       1066		FSHOW
1402:       0121		121			; 'Q'
1403:       1000		FQUIT
1404:       0103		103			; 'C'
1405:       1057		FCLEAR
1406:       0110		110			; 'H'
1407:       1003		FHELP
1410:       0077		77			; '?'
1411:       1003		FHELP
1412:       0054		54			; ','
1413:       1063		FDROP
1414:       0056		56			; '.'
1415:       1124		FPRINT
1416:       0053		53			; '+'
1417:       1133		FADD
1420:       0055		55			; '-'
1421:       1146		FSUB
1422:       0052		52			; '*'
1423:       1200		FMUL
1424:       0057		57			; '/'
1425:       1225		FDIV
1426:       0000		0			; end mark
1427:       0000	
1427:       0000	LINBUF:	0
1430:       0000		
1600:       0000		PAGE
1600:       0000	
1600:       0000	ENDBUF:	
1600:       0000		
1600:       0000	STACK:	0
1601:       0000	
1601:       0000	; eof

