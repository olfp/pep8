/*
   definitions for pot - pep8 opcode translator

   (c) olf 07/05
*/

#ifndef __POT__
#define __POT__

#include "types8.h"
#include "codes8.h"

#define FALSE	0
#define TRUE	!FALSE

#define PAGEMSK	07600		/* to mask the page out of an address */
#define ADDRMSK 00177		/* to mask the in page address out */
#define INPAGE	00200		/* zero page bit - set if not zero page! */
#define UNUSED  010000		/* internal flag if location has been used */

#define EQU	'='		/* Assign values to symbols */
#define ORG	'*'		/* Set address of generated code */
#define EOA	'$'		/* Terminate assembly */

#define DOT	'.'

#define SRC	".pps"
#define DST	".pmi"
#define LST	".pls"
#define SYM	".psy"

#define DELIMS	" \t\n"		/* whitespace characters */

#define BUFLEN	129		/* max. line length */

#define SYMSIZ	4096		/* default size of symbol table */

#define PLEN	10		/* maximum length of a pseudo mnemonic */

#define OPTCHR	'-'		/* option argument lead in */

#define STRTEX	'('		/* start arithmetic expression */
#define ENDEX	')'		/* end expression */

#define ARITHC	"+-*/"		/* arithmetic operands */

#define TERROR	0		/* token error */
#define TISBAD	1		/* token is a invalid (octal) number */
#define TISVAL	2		/* token is a (octal) number or symbol*/
#define TMNEMO	3		/* token is a mnemonic */

/* pseudo operations */

static void po_next_page(void);
static void po_text(void);

/* symbol table element */

typedef struct symtab_t {
  char symbol[SYMLEN+1];	/* name of symbol */
  WORD8 location;		/* associated address */
} SYMTAB;

typedef struct pseudo_t {
  char pseudo[PLEN+1];
  void  (*pseudoop)();
} PSEUDO;

PSEUDO pseudos[] = {
  "PAGE",	po_next_page,	/* advance location counter to next page */
  "TEXT",	po_text,	/* deposit SIXBIT text in memory */
  "",		0		/* table end marker */
};

/* prototypes */

static void symscan( char *line, FILE *symfile );
static int assemble( char *line, FILE *lstfile, WORD8 *assembly );
  
#endif








