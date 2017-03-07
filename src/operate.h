/*
   pdp8 simulator

   defs/decls for decoding OPR instructions
*/

#ifndef __OPERATE__
#define __OPERATE__

/* group bits */

#define OPRGRP1 00400			/* Group 1 vs. 2/3 bit for OPR */
#define OPRGRP2 00001			/* Group 2 / 3 bit for OPR */

/* group 1 */

#define PHASE1	00340			/* CLA, CLL, CMA, CML */
#define PHASE2	00017			/* RAL, RAR, IAC */

#define CLA	00200			/* clear AC */
#define CLL	00100			/* clear link bit */
#define CMA	00040			/* complement AC */
#define CML	00020			/* complement link bit */
#define RAR	00010			/* rotate right */
#define RAL	00004			/* rotate left */
#define DAC	00002			/* decrement <L,AC> */
#define IAC	00001			/* increment <L,AC> */

/* group 2 */

#define LOGGRP  00010			/* AND(1) or OR(0) group of bits */
#define CONDS	00160			/* Condition bits for skip */

/* OR group */

#define SMA	00100			/* skip on AC < 0 */
#define SZA	00040			/* skip on AC == 0 */
#define SNL	00020			/* skip on Link bit set */

#define SPA	00100			/* skip on AC > 0 */
#define SNA	00040			/* skip on AC */
#define SZL	00020			/* skip on link bit clear */

#define SKP	00010			/* skip always */
/* 00200 belongs to CLA (see group 1 */
#define OSR	00004			/* or switches with AC */
#define HLT	00002			/* halt */

/* group 3 */

#define IOPSHF	5			/* shift left five for iop */
#define DEVSHF	1			/* shift left one for devno */

#define IOPMSK	00340			/* i/o opcode mask */
#define DEVMSK	00036			/* i/o device mask */

#define SRI	00401			/* skip on ready for input */
#define SRO	00441			/* skip on read for output */
#define DGA	00501			/* dev get val to AC */
#define DPA	00541			/* dev put val in AC */
#define DUS	00601			/* select dev unit */
#define DGS	00641			/* dev get status word */
#define DSM	00701			/* dev set sense mask */
#define RSD	00741			/* dev reset */

#endif
