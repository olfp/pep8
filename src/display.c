/*
    pdp8 simulator

    display trace information
*/

#include <stdio.h>

#include "global.h"
#include "decode.h"
#include "util.h"

extern int submod;

void display() {

  WORD8 op, arg;
  WATCH *watch;

  /* display source line */

  if( usesource ) {
    if( (pc8 <= maxtxt) && text[pc8] && (text[pc8][0] != '\n')) {
      if(submod != 0) {
      	printf( "%s", text[pc8 - submod] );
      }
      printf( "%s", text[pc8] );
    } else {
      printf( "no source available for current address!\n" );
	  printf("Maxtxt %d, PC8: %d: %s\n", maxtxt, pc8, text[pc8]);
    }
  }

  /* show machine status */

  op = mem8[pc8];

  printf("pc: %04o: %04o - l:%1o ac: %04o", pc8, op, link8, ac8);

  /* decode operand if any */
  
  if( (op >> 9) < 5 ) {		/* RCL, STO, AND, ORA, TAD */
    arg = op & MASK7;
    printf( "\narg: %03o", arg );
    if( op & INPAGE ) {
      arg |= (pc8 & ~MASK7);
      printf( " -(%02o)-> %04o", pc8 >> 7, mem8[arg] );
    } else {
      printf( " -(ZP)-> %04o", mem8[arg] );
    }
    if( op & INDIRECT ) {
      arg = mem8[arg];
      printf( " -(I)-> %04o", mem8[arg] );
    }
  } else if( (op >> 9) < 7 ) {	/* JMS, JMP */
    arg = op & MASK7;
    if( op & INPAGE ) {
      arg |= (pc8 & ~MASK7);
    }
    printf( "\naddr: %04o", arg );
    if( op & INDIRECT ) {
      arg = mem8[arg];
      printf( " -(I)-> %04o", arg );
    }
  }
  
  putchar('\n');

  /* show watchlist */

  if(watched) {
    putchar('\n');
    for(watch = watched; watch; watch = watch->next) {
      if(watch->sym)
	printf("%s:\n", watch->sym);
      dumpmem( watch->addr, watch->cnt);
    }
  }
    
}

