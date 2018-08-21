/*
   opcodes for the pep8 aka permanent symbol table

   (c) olf 07/05
*/

#ifndef __CODES8__
#define __CODES8__

#include "types8.h"

#define MLEN 5			/* amx. length of a mnemonic */

typedef struct opcode_t {
  char	mnemonic[MLEN];
  WORD8	opcode;
} OPCODE;

OPCODE opcodes[] = {

  /* Mode modifiers */
  
  "I",   00400,			/* Indirect */

  /* Principal operations (with 7 Bit operand) */
  
  "RCL", 00000,			/* Recall Mem to Ac */
  "STO", 01000,			/* Store AC to Mem */
  "AND", 02000,			/* And AC and Mem */
  "ORA", 03000,			/* Or AC and Mem */
  "TAD", 04000,			/* Add Mem to <AC,L> */
  "JMP", 05000,			/* Jump */
  "JMS", 06000,			/* Subroutine transfer */
  "OPR", 07000,			/* Microcoded operation */

  /* Auxiliary instructions, Group 1, Bit 3 = 0 */

  "NOP", 07000,			/* Do nothing */

  "CLA", 07200,			/* Clear AC */
  "STA", 07240,			/* Set (Clear & Complement) AC */
  "CLL", 07100,			/* Clear Link Bit */
  "STL", 07120,			/* Set (Clear & Complement) Link */
  "CMA", 07040,			/* Complement AC */
  "CML", 07020,			/* Complement Link Bit */
  "IAC", 07001,			/* Increment <L,AC> */

  "RAR", 07010,			/* Rotate <L,AC> right */
  "RAL", 07004,			/* Rotate <L,AC> left */
  
  "STA", 07240,			/* Set AC to one */
  "STL", 07120,			/* Set Link Bit */
  "CIA", 07041,			/* 2's complement AC */

  /* Auxiliary instructions, Group 2, Bits 3 = 1 & 5 = 0 */

  "SKN", 07400,			/* Skip never */

  "SMA", 07440,			/* Skip on AC < 0 */
  "SZA", 07420,			/* Skip on AC == 0 */
  "SNL", 07410,			/* Skip on Link Bit set */

  "SKP", 07404,			/* Skip (always) */

  "SPA", 07444,			/* Skip on AC >= 0 */
  "SNA", 07424,			/* Skip on AC != 0 */
  "SZL", 07414,			/* Skip on Link Bit not set */

  "HLT", 07401,			/* Halt */

  /* Auxiliary instructions, Group 3 (I/O), Bits 3&4 = 1, 8-11 dev */

  "SRI", 07600,			/* Skip on ready for input */
  "SRO", 07620,			/* Skip on ready for output */
  "DGA", 07640,			/* Device get value to AC */
  "DPA", 07660,			/* Device put value from AC */
  "DUS", 07700,			/* Device unit select from AC */
  "DGS", 07720,			/* Device get status to AC */
  "DSM", 07740,			/* Device set sense mask from AC */
  "RSD", 07760,			/* Reset device */

  /* end of opcodes */

  "", 0
};

#endif
