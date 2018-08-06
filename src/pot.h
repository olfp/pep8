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
#define TISMAC	3		/* token is a mnemonic */
#define TMNEMO	4		/* token is a macro */

/* pseudo operations */

static void po_next_page(void);
static void po_text(void);
static void po_macro(void);
static void po_mend(void);

/* macros */

typedef struct macdef_t {
	char 			*line;	/* macro source line */
	struct macdef_t	*next;	/* pointer to next line */
} MACDEF;

/* symbol types */

typedef enum symtype_t {addr, macro} SYMTYPE;

/* symbol value */

typedef union symval_t {
  WORD8 	location;		/* associated address */
  MACDEF	*macdef;		/* pointer to macro definition */
} SYMVAL;

/* symbol table element */

typedef struct symtab_t {
  char symbol[SYMLEN+1];	/* name of symbol */
  SYMTYPE type;				/* type of symbol */
  SYMVAL val;				/* value of symbol */
} SYMTAB;

typedef struct pseudo_t {
  char pseudo[PLEN+1];
  void (*pseudoop)();
} PSEUDO;

PSEUDO pseudos[] = {
  "PAGE",	po_next_page,	/* advance location counter to next page */
  "TEXT",	po_text,	    /* deposit SIXBIT text in memory */
  "MACRO",	po_macro,		/* start macro definition */
  "ENDM",	po_mend,		/* end of macro definition */
  "",		0				/* table end marker */
};

/* prototypes */

static void symscan( char *line, FILE *symfile );
static int assemble( char *line, FILE *lstfile, WORD8 *assembly );
  
#endif








