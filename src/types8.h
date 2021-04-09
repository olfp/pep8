/*
   type definitions for pep8 emulation / assembler

   (c) olf 07/05
*/

#ifndef __TYPES8__
#define __TYPES8__

#define FALSE	0
#define TRUE	!FALSE

#define MASK12	07777		/* 12 Bit Mask */
#define MASK9	00777		/* 9 Bit Mask */
#define MASK8	00377		/* 8 Bit Mask */
#define MASK7	00177		/* 7 Bit Mask for addresses */
#define MASK6	00077		/* 6 Bit Mask for addresses */
#define MASK4	00017		/* 4 Bit Mask */
#define MASK3	00007		/* 3 Bit Mask */

#define SIGNBIT 04000		/* MSB is sign bit */

#define PAGE8	128             /* size of one page */
#define MAXPAG	31	        /* highes available page no. */

#define MEM8SIZ	4096		/* 12 Bit addressale space */

#define LBLCHR	",:"		/* Label delimiter */
#define COMM	"/;"		/* Comment lead in */

#define SYMLEN	6		/* max. length of symbols */

				/* WORD8 must be signed to detect overflow */
typedef int WORD8;		/* This is supposed to hold at least 13 bits */
typedef int BIT8;		/* This should hold a single bit */

#define isoctal(X) (((X) >= '0') && ((X) <= '7'))       /* check octal digit */

#endif
