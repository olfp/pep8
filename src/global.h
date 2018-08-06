/*
    pep8 simulator

    extern decls of global vars
*/

#ifndef __GLOBAL__
#define __GLOBAL__

#ifndef __TYPES8__
#include "types8.h"
#endif

#define IOBASE_DEFAULT	4200	/* where to base io tcp ports */

/* breakpoint list */

typedef struct brkpnt_t {
  WORD8 addr;
  struct brkpnt_t *next;
} BRKPNT;

/* symbol table element */

typedef struct symtab_t {
  char  symbol[SYMLEN+1];	/* name of symbol */
  int   location;		/* associated address */
  struct symtab_t *next;
} SYMBOL;

/* watch list element */

typedef struct watch_t {
  char *sym;			/* symbol or null */
  WORD8 addr;                   /* first word */
  unsigned int cnt;             /* so many words */
  struct watch_t *next;
} WATCH;

/* dump list element */

typedef struct dump_t {
  char *from;                   /* dump from (maybe sym) */
  unsigned int cnt;             /* so many words */
  struct dump_t *next;
} DUMP;

#ifndef __PEPSI__
extern WATCH *watched;
extern BRKPNT *brkfirst, *brklast;
extern SYMBOL *symbols;
#endif

/* general constants */

#define BUFLEN	129			/* max. line length */

#define ADRERR  -1			/* error paring address */
#define NOPARA  -2			/* no param found while parsing*/

#define TOKDEL	" \t,"			/* token delimiters */
#define	DEVDEL	":"			/* device delimeter */

#define PAGSEP	':'			/* for page:offset addresses */
#define DEVSEP	':'			/* device description sep */
#define ARGSEP	','			/* multi-argument separator */

/* elements of -8 engine */

extern WORD8 mem8[MEM8SIZ];		/* memory of pep8 */
extern WORD8 pc8;			/* current mem location */
extern WORD8 ac8;			/* accumulator */
extern BIT8  link8;			/* link bit */

extern int memtop;			/* highest touch mem location */

/* assorted masks */

#define LMASK   010000			/* Mask for Link Bit */

#ifndef __PEPSI__

/* simulator status flags */

extern int running;          		/* run or halt */
extern int interactive;			/* interactive mode */
extern int singlestep;        		/* pause after each instruction */
extern int verbose;           		/* print trace while processing */
extern int usesource;           	/* whether to use the source */
extern int coredump;           		/* whether to produce a coredump */

/* assembler source text buffer  */

extern int maxtxt;			/* addr of max. text line avail. */
extern char **text;			/* source text */

/* io port base */

extern int iobase;			/* tcp port base for dev io */

#endif

#endif

