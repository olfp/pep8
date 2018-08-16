/*
   pdp8 simulator

   defs/decls for decoding OPR instructions
*/

#ifndef __OPERATE__
#define __OPERATE__

/* group bits */

#define OPRGRP1 00400			/* Group 1 vs. 2/3 bit for OPR */
#define OPRGRP2 00200			/* Group 2 / 3 bit for OPR */

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

#define LOGGRP  00004			/* AND(1) or OR(0) group of bits */
#define CONDS	00240			/* Condition bits for skip */

/* OR group */

#define SMA	00040			/* skip on AC < 0 */
#define SZA	00020			/* skip on AC == 0 */
#define SNL	00010			/* skip on Link bit set */

#define SPA	00040			/* skip on AC > 0 */
#define SNA	00020			/* skip on AC */
#define SZL	00010			/* skip on link bit clear */

#define SKP	00004			/* skip always */
/* 00200 belongs to CLA (see group 1 */
#define OSR	00002			/* or switches with AC */
#define HLT	00001			/* halt */

/* group 3 */

#define IOPSHF	4			/* shift left five for iop */
#define DEVSHF	0			/* shift left one for devno */

#define IOPMSK	00160			/* i/o opcode mask */
#define DEVMSK	00017			/* i/o device mask */

#define SRI	00600			/* skip on ready for input */
#define SRO	00620			/* skip on read for output */
#define DGA	00640			/* dev get val to AC */
#define DPA	00660			/* dev put val in AC */
#define DUS	00700			/* select dev unit */
#define DGS	00720			/* dev get status word */
#define DSM	00740			/* dev set sense mask */
#define RSD	00760			/* dev reset */

#endif
