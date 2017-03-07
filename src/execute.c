/*
    pdp8 simulator

    process primary instructions
*/

#include "global.h"
#include "decode.h"

#include "execute.h"

extern int submod;

void execute( WORD8 opcode ) {

  WORD8 arg;				/* argument to categories 0-6 */

  submod = 0;				/* no offset for regular ops */

  if((opcode >> 9) < 7) {		/* opcodes that take an address */

    if( opcode & INPAGE ) {
      arg = opcode & MASK7 | (pc8 & ~MASK7);
    } else {
      arg = opcode & MASK7;
    }

    if( opcode & INDIRECT ) {
      arg = mem8[arg];
    }

    if( arg > memtop )
      memtop = arg;
  }

  switch( opcode >> 9 ) {		/* select by category */
  case RCL:                             /* recall AC from mem */
    ac8 = mem8[arg];
    pc8++;
    break;
  case STO:                             /* store AC to mem */
    mem8[arg] = ac8;
    pc8++;
    break;
  case AND:				/* logical and AC with mem*/
    ac8 &= mem8[arg];
    pc8++;
    break;
  case ORA:				/* logical or AC with mem */
    ac8 |= mem8[arg];
    pc8++;
    break;
  case TAD:				/* add to <L,AC> */
    ac8 += ( link8 + mem8[arg] );
    link8 = (ac8 & LMASK) ? 1 : 0;	/* if overflow set link bit */
    ac8 &= MASK12;
    pc8++;
    break;
  case JMS:				/* jump subroutine */
    mem8[arg] = pc8 + 1;		/* return address */
    pc8 = arg + 1;
    submod = 1;				/* check for next breakpoint 1 off */
    break;
  case JMP:				/* jump */
    pc8 = arg;
    break;
  case OPR:				/* microcoded operation */
    decodeopr(opcode);
    pc8++;
    break;
  }
}

