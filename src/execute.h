/*
   pdp8 simulator

   defs/decls for executing insttructions
*/

#ifndef __EXECUTE__
#define __EXECUTE__

/* opcode categories */

#define RCL	0			/* Recall AC */
#define STO	1			/* Store AC */
#define AND	2			/* And AC */
#define ORA	3			/* Or AC */
#define	TAD	4			/* Add to <L,AC> */
#define	JMP	5			/* Jump */
#define JMS	6			/* Call Subroutine */
#define	OPR	7			/* Microcoded operation */

#endif
