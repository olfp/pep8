/*
    pep8 simulator

    process OPR instructions
*/

#include <stdio.h>

#include "global.h"
#include "chario.h"
#include "operate.h"

void decodeopr( WORD8 opcode ) {

  int flag, disp, iop, dev;

  if( opcode & OPRGRP1 ) {		/* bit is 1 for Group 2 & 3 */
    if( opcode & OPRGRP2 ) {		/* bit set for Group 3 */
      iop = (opcode & IOPMSK) >> IOPSHF;
      dev = (opcode & DEVMSK) >> DEVSHF;
      if(dev == CPUDEV) {		/* virtual cpu device */
	if(opcode & RSD) {			/* dev reset -> halt */
	  running = FALSE;		/* stop CPU */
	  pc8--;                        /* make pc8 point to HLT instr. */
	}
      } else {
	if(chario(iop, dev, &ac8)) {
	  pc8++;			/* skip */
	}
      }
    } else {				/* its a group 2 instruction */
      if( opcode & CONDS ) {		/* is it a conditional skip? */
	flag = FALSE;			/* Default: OR Group */
	if( opcode & SMA )
	  flag = flag || (ac8 & SIGNBIT); /* is Ac < 0 (MSB set) ? */
	if( opcode & SZA )
	  flag = flag || !ac8;        	/* is AC == 0 ? */
	if( opcode & SNL )
	  flag = flag || link8;         /* is the link bit set? */
	if( opcode & LOGGRP ) 		/* AND group */
	  flag = !flag;			/* Don't skip if any OR Bit set */
	  
	if( flag ) {			/* shall we skip? */
	  disp = (opcode & DSPMSK);	/* displacement */
	  if(disp)
	    pc8 -= (disp + 1);		/* branch back */
	  else 
	    pc8++;			/* skip next */
	}
      }
    }
  } else {				/* its a group 1 instruction */
    if( opcode & CLA )
      ac8 = 0;				/* clear AC */
    if( opcode & CLL )
      link8 = 0;			/* clear link bit */
    if( opcode & CMA ) 
      ac8 = ~ac8 & MASK12;		/* complement AC */
    if( opcode & CML )
      link8 = link8 ? 0 : 1;            /* complement L */
    if( opcode & RAR ) {
      ac8 |= link8 << 12;	        /* old link is 13th bit */
      link8 = ac8 & 1;		        /* new link bit is LSB of accu*/
      ac8 = ac8 >> 1;		        /* shift new ac in place */
    } if( opcode & RAL ) {
      ac8 = (ac8 << 1) | link8;	        /* shift accu left, put link in LSB */
      link8 = (ac8 & LMASK) ? 1 : 0;    /* set link according to 13th bit */
      ac8 &= MASK12;		        /* reset accu to 12 bits */
    }
    if( opcode & DAC )  {
      ac8--;                            /* decrement <L,AC> */
      if( ac8 < 0 )
        link8 = ~link8;
      ac8 &= MASK12;
    }
    if( opcode & IAC )  {
      ac8++;                            /* increment <L,AC> */
      if( ac8 > MASK12 )
        link8 = ~link8;
      ac8 &= MASK12;
    }
  }
}

