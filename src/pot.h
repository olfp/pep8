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
#define DOT	'.'		/* value of current pc */
#define PRM	'\\'	/* Macro param lead in */
#define MNO	'%'		/* Macro no in labels */
#define EOA	'$'		/* Terminate assembly */

#define DOT	'.'
#define SEP '/'

#define SRC	".pps"
#define DST	".pmi"
#define LST	".pls"
#define SYM	".psy"

#define TMPNAMT "/tmp/potXXXXXX"

#define DELIMS	" \t\n"		/* whitespace characters */

#define BUFLEN	129		/* max. line length */
#define OLILEN	BUFLEN+SYMLEN-2 /* max line + inserted symbol - macro param */

#define SYMSIZ	4096	/* default size of symbol table */

#define PLEN	10		/* maximum length of a pseudo mnemonic */

#define OPTCHR	'-'		/* option argument lead in */

#define STRTEX	'('		/* start arithmetic expression */
#define ENDEX	')'		/* end expression */

#define ARITHC	"+-*/&"		/* arithmetic operands */

#define TERROR	0		/* token error */
#define TISBAD	1		/* token is a invalid (octal) number */
#define TISVAL	2		/* token is a (octal) number or symbol*/
#define TISMAC	3		/* token is a mnemonic */
#define	TISPRM	4		/* token is a macro positional param */
#define TMNEMO	5		/* token is a macro */

#define MAXPARA 10		/* max macro params */

#define MAXNEST	10		/* file include deepth */

#define ZMEMMAX 127		/* last word in zero page */

/* pseudo operations */

static void po_next_page(void);
static void po_text(void);
static void po_zmem(void);
static void po_dubl(void);
static void po_fltg(void);
static void po_macro(void);
static void po_mend(void);
static void po_file_incl(void);

/* macros */

typedef struct strlst_t {
	char 			*str;	
	struct strlst_t	*next;	
} STRLST;

typedef struct macpara_t {
	char 	*name;			/* name or value in invocation */
	WORD8	val;			/* preassembled value */
} MACPARA;	

/* symbol types */

typedef enum symtype_t {addr, macro} SYMTYPE;

/* symbol value */

typedef union symval_t {
  WORD8 	location;		/* associated address */
  STRLST	*macdef;		/* pointer to macro definition */
} SYMVAL;

/* symbol table element */

typedef struct symtab_t {
  char symbol[SYMLEN+1];	/* name of symbol */
  SYMTYPE type;				/* type of symbol */
  int refcnt;				/* reference counter */
  SYMVAL val;				/* value of symbol */
} SYMTAB;

typedef struct pseudo_t {
  char pseudo[PLEN+1];
  void (*pseudoop)();
} PSEUDO;

PSEUDO pseudos[] = {
  "PAGE",	po_next_page,	/* advance location counter to next page */
  "TEXT",	po_text,	    /* deposit SIXBIT text in memory */
  "ZMEM",	po_zmem,	    /* allocate zero page constant */
  "DUBL",	po_dubl,		/* deposit 24 bit balue in memory, big endian */
  "FLTG",	po_fltg,		/* deposit 36 bit ffp8 float, 12 bit exponent and 24 bit big endian mantissa */
  "MACRO",	po_macro,		/* start macro definition */
  "ENDM",	po_mend,		/* end of macro definition */
  "FILE",	po_file_incl,	/* include file */
  "",		0				/* table end marker */
};

/* prototypes */

static void macnify(char *tok);
static int valueof(char *tok, char *line, unsigned *val);
static void symscan( char *line, FILE *symfile );
static int assemble( char *line, FILE *lstfile, FILE*tmpfile, WORD8 *assembly );
  
#endif








