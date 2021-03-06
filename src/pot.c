/*
 * pot - pep8 opcode translator
 * 
 * (c) olf 07/05, 2021
 * 
 * takes .pps file to produce an .pmi image file to be loaded into pepsi(1)
 * 
 * missing features?: 	1) more pseudos: DUBL (24 bit int)
 *			2) FLTG (36 Bit floating point)
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>

#include "strutil.h"
#include "octal.h"
#include "pot.h"

WORD8	mem8[MEM8SIZ];		/* memory of pep8 */
WORD8	pc8;			/* current mem location */

int	memtop;			/* highest touched mem location */

SYMTAB  *symtab;		/* symbol table */
SYMTAB  *symptr;		/* current symbol */
SYMTAB	*curmac;		/* currently invoked  macro symbol */

int	symsiz = SYMSIZ;	/* size of symbol table */

int	verbose = FALSE;	/* more or less output */

int	symout = FALSE;		/* write symbol table to file */
int	lstout = FALSE;		/* write listing to file */

int	lineno = 0;		/* current linenumber */
int	errcnt = 0;		/* number of errors */

int	run = TRUE;		/* reset if end of assemble char seen */
int	pass = 0;		/* assemler pass; 1=symscan, 2=assemble */

int	textmode = 0;		/* assemble nums/sysm or text/sixbit */
int	macromode = 0;		/* true if defining macro */
int	macrohead = 0;		/* true if MACRO line */

STRLST	*macroptr = NULL; 	/* assembly active macro line */

STRLST 	*macline = NULL;  	/* pointer to current macro line */
MACPARA macpara[MAXPARA]; 	/* mac invocation params */

WORD8	zmemptr = ZMEMMAX;	/* next free zero page constant addr */
STRLST	*zlabel[ZMEMMAX+1];	/* labels defined by zmem */

FILE	*infile;		/* active input file */
char 	*inpath;		/* path to input file */
FILE 	*files[MAXNEST];	/* input file stack for includes */
int  	filetop = 0;		/* stack ptr for file stack */
char 	oline[OLILEN];		/* current input line case preserved */
char 	uline[BUFLEN];		/* current input line upper cased */

extern int  optind;
extern char *optarg;	/* argument pointer for getopt */

static void 
usage(char *name)
{
	fprintf(stderr, "%s - pep8 opcode translator\n", name);
	fprintf(stderr, "usage: %s [-lsS<n>] <infile>[.pps]\n", name);
	fprintf(stderr, "  v - verbose - tell what's going on\n");
	fprintf(stderr, "  l - write a .pls listing file\n");
	fprintf(stderr, "  s - write symbols to a .psy file\n");
	fprintf(stderr, "  S - set size of symbol to <n> (default: %d)\n", SYMSIZ);
	exit(EXIT_FAILURE);
}

static void 
symtab_insert(char *sym, SYMVAL val, SYMTYPE type) 
{
  SYMTAB *symtmp;

  if (strlen(sym) > SYMLEN) {
    fprintf(stderr, "%s: symbol too long. Truncated.\n", sym);
    sym[SYMLEN] = '\0';
  }

  /* check if symbol already exists */
  for (symtmp = symtab; symtmp != symptr; symtmp++) {
    if (!strcmp(symtmp->symbol, sym)) {
      fprintf(stderr, "Duplicate symbol %s.\n", sym);
      errcnt++;
      break;
    }
  }
	
  /* check symbol tabe overflow */
  if (symptr >= symtab + symsiz) {
    fprintf(stderr, "Fatal: symbol table overflow, line %05d!\n", lineno);
    exit(EXIT_FAILURE);
  }

  /* insert symbol */
  symptr->type = type;
  symptr->refcnt = 0;
  strcpy(symptr->symbol, sym);			
  symptr->val = val;
}

static void
symtab_next()
{
  symptr++;
  *(symptr->symbol) = '\0';	/* mark end of table */
}

static void 
macroline(char *line)
{
  STRLST *def;
	
  def = (STRLST *)malloc(sizeof(STRLST));
  def->str = strdup(line);
  def->next = NULL;
	
  if(macline) {
    macline->next = def;
  } else {
    symptr->val.macdef = def;
  }	 
  macline = def;
}

static void 
po_next_page()
{

  pc8 &= ~MASK7;
  pc8 += PAGE8;
  if (pc8 > MASK12) {
    fprintf(stderr, "Fatal: page %o is out of range!\n", pc8 >> 7);
    exit(EXIT_FAILURE);
  }
}

static void 
po_text()
{
  textmode = 0xff;
}

static void 
po_zmem()
{
  SYMVAL symbuf;
  char *name, *tok;
  int type;
  unsigned val;
  WORD8 ptr;
  STRLST *zlbl;

  name = strtok(NULL, DELIMS);
  tok = strtok(NULL, DELIMS);
	
  if(pass == 2 && !macromode) {		
    type = valueof(tok, uline, &val); 

    for(ptr = ZMEMMAX; ptr > zmemptr; ptr--) {
      if(mem8[ptr] == val) {
	break;
      }
    }
    symbuf.location = ptr;

    if(strchr(name, MNO)) { /* symbol has a macro ref placeholder (%+) */
      macnify(name);
    }
    zlbl = (STRLST *)malloc(sizeof(STRLST));
    zlbl->str = strdup(name);
    zlbl->next = zlabel[ptr];
    zlabel[ptr] = zlbl;
    if(ptr == zmemptr) {
      mem8[zmemptr--] = val;	
    }

    symtab_insert(name, symbuf, addr);		
    symtab_next();
  }
}

static void 
po_dubl()
{
  /* ni */
}

static void 
po_fltg()
{
  /* ni */
}

static void 
po_macro()
{
  char *name;
  SYMVAL symbuf;
	
  if(pass == 1) {
    name = strtok(NULL, DELIMS);
    symbuf.macdef = NULL;
    symtab_insert(name, symbuf, macro);		
    macrohead = TRUE;					
  }
  macromode = TRUE;
}

static void 
po_mend()
{
  if(pass == 1) {
    symtab_next();
    macline = NULL;
  }
  macromode = FALSE;
}

static void 
po_file_incl()
{
  char *ufile, *file, *inname, obuf[BUFLEN];
  int len;

  /* go to some length to get teh case preserved file name ... */
  ufile = strtok(NULL, DELIMS);
  strcpy(obuf, oline + (ufile - uline));
  file = strtok(obuf, DELIMS);
  if(filetop >= MAXNEST) {
    fprintf(stderr, "FILE %s: Nesting too deep (%d).\n", file, filetop);
    exit(EXIT_FAILURE);
  } 
  len = (inpath ? strlen(inpath) : 0) + strlen(file) + strlen(SRC) + 1;
  inname = (char *)malloc(len);
  if(inpath) {
    strcpy(inname, inpath);
  } else {
    *inname = '\0';
  }
  strcat(inname, file);
  strcat(inname, SRC);
  files[++filetop] = infile;
  if ((infile = fopen(inname, "r")) == NULL) {
    fprintf(stderr, "FILE %s: File %s not found.\n", file, inname);
    exit(EXIT_FAILURE);
  }
  if(verbose)
    printf("INFO: Including file: %s.\n", inname);
}

static int 
pseudoop(char *tok)
{

  PSEUDO         *pop;

  for (pop = pseudos; *(pop->pseudo) != '\0'; pop++) {
    if (!strcmp(tok, pop->pseudo)) {
      (pop->pseudoop) ();
      break;
    }
  }

  return (*(pop->pseudo) != '\0');
}

static void
macnify(char *tok)
{
  char *first, *last, fmt[SYMLEN+1];
  int len;
	
  first = strchr(tok, MNO);
  last = strrchr(tok, MNO);
  len = last - first + 1;
  /* strlcpy(fmt, tok, SYMLEN);  *** not inn linux */
  strcpy(fmt, tok);
  sprintf(fmt + (first - tok), "%%0%dd", len);
  sprintf(tok, fmt, curmac->refcnt);
  strncpy(oline + (tok - uline), tok, strlen(tok));
}

static void 
symscan(char *line)
{

  char	*tok, *nxt;
  char 	lbuf[BUFLEN];
  int		flag;
  int		lblfld = TRUE;
  int		i, cnt = 0;
  unsigned ibuf;
  SYMTAB  *symtmp;
  SYMVAL	symbuf;
	
  tok = strtok(line, DELIMS);

  while (tok != NULL) {
    nxt = tok + strlen(tok) + 1;

    /* check for end-of-assembly symbol */

    if (*tok == EOA) {
      run = FALSE;	/* stop assembly */
      break;	/* exit processing this line */
    }
    /* check for comment */

    if (strchr(COMM, *tok))
      break;	/* end processing this line */

		/* check for new target address */

    if (*tok == ORG) {
      if (checkoctal(++tok, 4)) {
	sscanf(tok, "%o", &ibuf);
	pc8 = (WORD8) ibuf;
      } else {
	fprintf(stderr, "%05d: bad octal number: %s\n", lineno, tok);
	errcnt++;
      }
      break;	/* done with this line */
    }
    if (lblfld && !macromode && (strchr(LBLCHR, tok[strlen(tok) - 1]) || strchr(tok, EQU))) {

      /* process labels and assignments */

      flag = (strchr(LBLCHR, tok[strlen(tok) - 1]) != NULL);
      tok[strlen(tok) - 1] = '\0';	/* eleminate the label * delimiter */
      if(strchr(tok, MNO)) { /* label has a macro no placeholder (%+) */
	macnify(tok);
      }

      if (flag) {
	symbuf.location = pc8;
      } else {
	assemble(nxt, NULL, NULL, &symbuf.location);
      }

      symtab_insert(tok, symbuf, flag ? addr : litr);			
      symtab_next();

      if (!flag)	/* value symbol */
	break;	/* line is completely processed by assemble */

    } else {

      /* there is actual code in this line */
      if (pseudoop(tok)) {	/* process pseudo operations */
	if (textmode) {
	  tok += strlen(tok) + 1;
	  while (*tok && strchr(DELIMS, *tok)) {
	    tok++;
	  }
	  if (*tok) {
	    textmode = *tok++;
	    pc8++;	/* length */
	    cnt++;
	    while (*tok && (*tok != textmode)) {
	      pc8++;
	      cnt++;
	      tok++;
	      if (*tok && (*tok != textmode))
		tok++;
	    }
	  }
	  textmode = 0;
	}
      } else {
	lblfld = FALSE;	/* none found, no more labels now */
				
	/* check for macro invocation */ 
	for (symtmp = symtab; symtmp < symptr; symtmp++) {
	  if (!strcmp(tok, symtmp->symbol) && (symtmp->type == macro)) {
	    curmac = symtmp;
	    symtmp->refcnt++;
	    macroptr = symtmp->val.macdef;
	    while(macroptr) {
	      /* pc8++; */
	      strcpy(oline, macroptr->str);
	      strucpy(uline, oline);
	      symscan(uline);
	      macroptr = macroptr->next;
	    } 
	    lblfld = TRUE;	
	  }
	}
      }
      break;	/* done with this line */
    }

    tok = strtok(NULL, DELIMS);
  }

  if (!lblfld && !macromode) {		/* if code was generated */
    ++pc8;
  }
}

static int 
valueof(char *tok, char *line, unsigned *val)
{

  OPCODE    *code;
  SYMTAB    *symtmp;
  char			*first, *last, *pos, *name;
  int		  	len, off, num, type = TERROR;

  if (isdigit(*tok) || (strchr("+-", *tok) && isdigit(*(tok + 1)))) {
    /* its a number! */
    if (checkoctal(tok, 4)) {
      sscanf(tok, "%o", val);
      type = TISVAL;
    } else {
      type = TISBAD;
    }
  } else if( *tok == PRM ) {
    pos = tok;
    num = *(++tok) - '1';
    if((num > 9) || *(++tok)) {
      type = TERROR;
    } else {
      type = TISPRM;
      *val = macpara[num].val;
      /* for listing, replace "\x" with param name/value in orig line */
      first = oline + (pos - line);
      last = oline + strlen(oline);
      name = macpara[num].name;
      len = strlen(name);
      if(len > 1) {
	off = len - 2; /* -2 is ref len "\x" */
	for(pos = first + 2; isblank(*pos) && off; pos++) {
	  off--;
	}	 		
	if(off) {
	  for(pos = last; pos >= first; pos--) {
	    *(pos+off) = *pos;
	  }
	}
	memcpy(first, name, len);
      } else {
	/* name is one char long */
	*first = *name;
	*(first+1) = ' ';
      }
    }
  } else if( *tok == DOT ) {
    type = TISVAL;
    *val = pc8;
  } else {
    /* check for mnemonic or symbol */
    for (code = opcodes; *(code->mnemonic) != '\0'; code++) {
      if (!strcmp(tok, code->mnemonic)) {
	type = TMNEMO;
	*val = code->opcode;
	break;
      }
    }
    if (*(code->mnemonic) == '\0') {	/* no mnemonic */
      /* must be a symbol or macro*/
      if(strchr(tok, MNO)) { /* symbol has a macro ref placeholder (%+) */
	macnify(tok);
      }
      if( *tok == BRALBL ) {		/* branch displacement */
	type = TISDSP;
	*val = pc8;
	tok++;
      }
      for (symtmp = symtab; symtmp < symptr; symtmp++) {
	if (!strcmp(tok, symtmp->symbol)) {
	  symtmp->refcnt++;
	  if((symtmp->type == addr) || (symtmp->type == litr)) {
	    if(type == TISDSP) {
	      *val -= symtmp->val.location;
	      if(*val > MAXDSP) {
		type = TISOVR;
	      } else {
		type = TISVAL;
	      }
	    } else {
	      *val = symtmp->val.location;
	      type = TISVAL;
	    }
	  } else if(symtmp->type == macro) {
	    type = TISMAC;
	    *val = 0;
	    curmac = symtmp;
	    macroptr = symtmp->val.macdef;
	  } else {
	    type = TISBAD;
	  }
	  break;
	}
      }
    }
  }

  return type;
}

static FILE*
lstotmp(FILE *lst, FILE* tmp) {
  return (pc8 > ZMEMMAX) ? tmp : lst;
}

static int 
assemble(char *line, FILE *lstfile, FILE *tmpfile, WORD8 * assembly)
{

  char        *tok;
  WORD8		pos, op, loc = UNUSED;
  OPCODE      *code;
  SYMTAB      *symptr;
  char		errstr    [6] = "";
  int			lblfld = TRUE;
  int 		mac = FALSE;
  int			len, cnt = 0;
  unsigned 	ibuf, newval, val;
  int			type, val6, parapos;
  char        *close, *next;
  char		prevop   , opchar;
	
  tok = strtok(line, DELIMS);

  while (tok != NULL) {

    /* check for end-of-assembly symbol */

    if (*tok == EOA) {
      run = FALSE;	/* stop assembly */
      break;	/* exit processing this line */
    }
    /* check for new target address */

    if (*tok == ORG) {
      if (checkoctal(++tok, 4)) {
	sscanf(tok, "%o", &ibuf);
	pc8 = (WORD8) ibuf;
      } else {
	fprintf(stderr, "%05d: bad octal number: %s\n", lineno, tok);
	errcnt++;
	strcat(errstr, "N");
      }
      break;	/* done with this line */
    }
    /* check for comment or assignment */

    if (strchr(COMM, *tok) || strchr(tok, EQU))
      break;	/* end processing this line */

    if (lblfld && strchr(LBLCHR, tok[strlen(tok) - 1])) {

      /* process labels */
      if(!macromode && strchr(tok, MNO)) { 	/* symbol has a macro ref placeholder (%+) */
	macnify(tok);		/* apply ref num for listing */
      }
    } else {

      if (pseudoop(tok)) {	/* process pseudo ops */
	if (textmode) {
	  tok += strlen(tok) + 1;
	  while (*tok && strchr(DELIMS, *tok)) {
	    tok++;
	  }
	  if (*tok) {
	    textmode = *tok++;
	    len = strrchr(tok, textmode) - tok;
	    if (lstfile)	  
	      fprintf(lstotmp(lstfile, tmpfile), "%04o: %-5s %04o\t%s", pc8, errstr, len, oline);
	    mem8[pc8++] = len;
	    cnt++;
	    while (*tok && (*tok != textmode)) {
	      val6 = ((*tok - 32) << 6) | (*(tok + 1) - 32);
	      if (lstfile)
		fprintf(lstotmp(lstfile, tmpfile), "%04o:       %04o\n", pc8, val6);
	      mem8[pc8++] = val6;
	      cnt++;
	      tok++;
	      if (*tok && (*tok != textmode))
		tok++;
	    }
	  } else {
	    fprintf(stderr, "No text at TEXT pseudo!\n");
	  }
	  textmode = 0;
	}
	break;	/* rest of line is ignored */
      }
			
      if(macromode) {
	assembly = 0;
	tok = NULL;
	break;
      }
			
      /* process mnemonics & operands */

      lblfld = FALSE;	/* no more labels now */

      if (*tok == STRTEX) {	/* arithmetic expression */
	type = TISBAD;	/* default to bad value */
	if (!(close = strchr(tok, ENDEX))) {
	  fprintf(stderr, "%05d: unterminated expression %s\n", lineno, tok);
	  errcnt++;
	  strcat(errstr, "U");
	} else {
	  *close = '\0';
	  (void)*tok++;
	  val = 0;
	  prevop = '+';
	  while (*tok) {
	    next = tok;
	    if (strchr(ARITHC, *next))
	      next++;	/* skip leading signs */
	    while (*next && !strchr(ARITHC, *next)) {
	      next++;
	    }
	    opchar = *next;
	    *next++ = '\0';
	    valueof(tok, line, &newval);
	    switch (prevop) {
	    case '*':
	      val *= newval;
	      break;
	    case '/':
	      val /= newval;
	      break;
	    case '+':
	      val += newval;
	      break;
	    case '-':
	      val -= newval;
	      break;
	    case '&':
	      val &= newval;
	      break;
	    }
	    prevop = opchar;
	    tok = next;
	  }
	  type = TISVAL;
	}
      } else {
	type = valueof(tok, line, &val);
      }

      if (type == TMNEMO) {	/* its an opcode */
	if (loc & UNUSED)
	  loc = val;
	else
	  loc |= val;
      }
      if (type == TISVAL || type == TISPRM) {	/* its a valid number, param or * symbol */
	op = (WORD8) val;
	if (loc & UNUSED) {
	  loc = op;
	} else {
	  if ((op & PAGEMSK)) {	/* in-page address? */
	    if ((op & PAGEMSK) != (pc8 & PAGEMSK)) {	/* not this page? */
	      fprintf(stderr, "%05d: off page reference %s\n", lineno, tok);
	      errcnt++;
	      strcat(errstr, "P");
	    }
	    loc |= INPAGE;
	  }
	  loc |= (op & ADDRMSK);
	}
      }
      if (type == TISMAC) {
	mac = TRUE;
	strcat(errstr, "M");
	for(parapos = 0; parapos < MAXPARA; parapos++) {
	  macpara[parapos].val = UNUSED;
	}
	parapos = 0;
      }
      if (type == TISOVR) {	/* displacemnt overflow */
	fprintf(stderr, "%05d: Displacement exceeds limit: %s\n", lineno, tok);
	errcnt++;
	strcat(errstr, "D");
      }
      if (type == TISBAD) {	/* its an invalid number */
	fprintf(stderr, "%05d: bad octal number: %s\n", lineno, tok);
	errcnt++;
	strcat(errstr, "N");
      }
      if (type == TERROR) {	/* its a nothing, probably an undef sym */
	fprintf(stderr, "%05d: undefined symbol: %s\n", lineno, tok);
	errcnt++;
	strcat(errstr, "S");
      }
    }
    if(mac && (type != TISMAC)) {
      macpara[parapos].val = loc;
      macpara[parapos].name = strdup(tok);
      parapos++;
      loc = 0;
    }

    tok = strtok(NULL, DELIMS);
  }

  loc &= MASK12;

  if (lstfile && !cnt)
    fprintf(lstotmp(lstfile, tmpfile), "%04o: %-5s %04o\t%s", pc8, errstr, loc, oline);
		

  if (assembly)		/* return generated bits if requested */
    *assembly = loc;

  if (!lblfld && !cnt && !mac && !assembly)	{/* did code, no textmode, no sympass */
    mem8[pc8++] = loc;
  }

  return (!lblfld);	/* return true if code was generated */
}

static char *nextline(char *buf, int len) {
  char * line;
	
  if(macroptr) {
    strncpy(buf, macroptr->str, len);
    macroptr = macroptr->next;
    return buf;
  } else {
    line = fgets(buf, len, infile);	
    if(!line && filetop) { /* we are in an include file */
      fclose(infile);
      infile = files[filetop--];
      line = fgets(buf, len, infile);	
    }

    return line;
  }
}

int 
main(int argc, char *argv[])
{

  FILE    *outfile, *lstfile, *tmpfile, *symfile;
  char    *inname, *outname, *lstname, *tmpname, *symname, *tmptmpl;
  char	  *lastsep, *tmpline;
  char    *prog, *file;
  int	  optch;
  STRLST  *zlbl;
  SYMTAB  *symtmp;
  WORD8   loc;
  int	  i, j, f, l, len, tmpfd;
  unsigned char oc[3];

  prog = argv[0];
  if (!(prog = strrchr(prog, '/'))) {
    prog = argv[0];
  } else {
    prog++;
  }

  while ((optch = getopt(argc, argv, "vlsS:?")) > 0) {
    switch ((char)optch) {
    case 'v':	/* verbose */
      verbose = TRUE;
      break;
    case 'l':	/* write listing file */
      lstout = TRUE;
      break;
    case 's':	/* write symbol table to file */
      symout = TRUE;
      break;
    case 'S':	/* Symbol table size */
      symsiz = atoi(optarg);
      break;
    default:
      usage(prog);
      break;
    }
  }

  if (optind >= argc) {
    usage(prog);
  }
	
  if(verbose)
    printf("INFO: %s - pep8 opcode translator.\n", prog);
	
  /* process filename given */

  file = argv[optind];

  if (strchr(file, DOT)) {
    len = strlen(file) + 4;	/* just to be sure take a few bytes more */
    inname = (char *)malloc(len);
    strcpy(inname, file);
  } else {
    len = strlen(file) + strlen(SRC) + 1;
    inname = (char *)malloc(len);
    strcpy(inname, file);
    strcat(inname, SRC);
  }
	
  if(verbose)
    printf("INFO: Input file: %s\n", inname);
	
  inpath = strdup(inname);
  if((lastsep = strrchr(inpath, SEP))) { /* xtra parens for gcc */
    *(++lastsep) = '\0';
  } else {
    inpath = NULL;
  }

  outname = (char *)malloc(len);
  strcpy(outname, inname);
  strcpy(strrchr(outname, DOT), DST);

  if (lstout) {
    lstname = (char *)malloc(len);
    strcpy(lstname, inname);
    strcpy(strrchr(lstname, DOT), LST);
    if(verbose)
      printf("INFO: Listing file: %s\n", lstname);
  }
  if (symout) {
    symname = (char *)malloc(len);
    strcpy(symname, inname);
    strcpy(strrchr(symname, DOT), SYM);
    if(verbose)
      printf("INFO: Symbol file: %s\n", symname);
  }
  /* open input file */

  if ((infile = fopen(inname, "r")) == NULL) {
    fprintf(stderr, "%s: File %s not found.\n", argv[0], inname);
    exit(EXIT_FAILURE);
  }
  files[0] = infile; /* push base file to bottom of include stack */
	
  for(i = 0; i < ZMEMMAX; i++) {
    zlabel[i] = NULL;
  }
	
  pass++;
  if(verbose)
    printf("INFO: First pass, scanning symbols...\n");
	
  /* make symbol table */

  symptr = symtab = (SYMTAB *) malloc(symsiz * sizeof(SYMTAB));
  *(symptr->symbol) = '\0';	/* mark end of table */

  memtop = pc8 = 0;	/* start at loc zero by default */

  while (run && nextline(oline, BUFLEN)) {
    strucpy(uline, oline);
    lineno++;
    symscan(uline);
    if (macromode && !macrohead) {
      macroline(oline);
    }
    macrohead = FALSE;
  }

  if (errcnt > 0) {
    fprintf(stderr, "%d error(s) after pass 1. Abort.\n", errcnt);
    exit(EXIT_FAILURE);
  }	
	
  for(symtmp = symtab; symtmp < symptr; symtmp++) {
    symtmp->refcnt = 0;
  }
	
  /*
    for(symtmp = symtab; symtmp < symptr; symtmp++) {
    if(symtmp->type == macro) {
    STRLST *def;
    printf("MACRO: %s\n", symtmp->symbol);
    def = symtmp->val.macdef;
    while(def) {
    printf("%s", def->str);
    def = def->next;
    }
    printf("ENDM: %s\n", symtmp->symbol);
    } else {
    printf("SYMBOL: %s - %o\n", symtmp->symbol, symtmp->val.location);							
    }
    }
  */
	
  if(verbose)
    printf("INFO: Found %ld symbols and macro definitions on %d lines.\n", (symptr - symtab), lineno);
	
	
  /* Pass 1 done */
  pass++;
  if(verbose)
    printf("INFO: Second pass, assembling code...\n");

  /* initialize mem buffer / loc pointer */

  for (j = 0; j < MEM8SIZ; j++)
    mem8[j] = 0;	/* zap memory buffer */

  memtop = pc8 = 0;	/* start at loc zero by default */

  /* rewind input file */

  rewind(infile);
  lineno = 0;

  if (lstout) {
    if ((lstfile = fopen(lstname, "w")) == NULL) {
      fprintf(stderr, "%s: Error opening list file %s.\n", argv[0], lstname);
      exit(EXIT_FAILURE);
    }
    tmptmpl = strdup(TMPNAMT);
    if ((tmpfd = mkstemp(tmptmpl)) && (tmpfile = fdopen(tmpfd, "w+")) == NULL) {
      fprintf(stderr, "%s: Error opening listing temp file %s.\n", argv[0], tmptmpl);
      exit(EXIT_FAILURE);
    }
    unlink(tmptmpl); /* unlink-on-close */
  } else {
    lstfile = NULL;	/* to preven output in assemble() */
  }

  /* do the assembly */

  run = TRUE;

  while (run && nextline(oline, BUFLEN)) {
    strucpy(uline, oline);
    lineno++;
    /* write post zero page listing in tmp file, to insert the zmem list later */
    if (assemble(uline, lstfile, tmpfile, NULL)) {
      if (pc8 > memtop)
	memtop = pc8;	/* keep track of user mem */
    }
  }

  if(verbose)
    printf("INFO: Allocated %d words in memory.\n", memtop);
	
  /*
    for(symtmp = symtab; symtmp < symptr; symtmp++) {
    if(symtmp->type == macro) {
    STRLST *def;
    printf("MACRO: %s\n", symtmp->symbol);
    def = symtmp->val.macdef;
    while(def) {
    printf("%s", def->str);
    def = def->next;
    }
    printf("ENDM: %s\n", symtmp->symbol);
    } else {
    printf("SYMBOL: %s: %o REF %d\n", symtmp->symbol, symtmp->val.location, symtmp->refcnt);							
    }
    }
  */
	
  if (lstfile) {
    /* add zmem lst to lstfile */
    while(++zmemptr <= ZMEMMAX) {	
      tmpline = oline;
      zlbl = zlabel[zmemptr];
      while(zlbl) { 
	loc = (zlbl->next ? 0 : mem8[zmemptr]);
	fprintf(lstfile, "%04o: %-5s %04o\t%s:\t%04o\n", zmemptr, "", loc, zlbl->str, mem8[zmemptr]);
	zlbl = zlbl->next;
      }
    }		
		
    /* append tmp file to lst file */
		
    freopen(NULL, "r", tmpfile);
    while(fgets(oline, OLILEN, tmpfile)) {
      fputs(oline, lstfile);
    }
    fclose(tmpfile);

    fputc('\n', lstfile);
    fclose(lstfile);
  }
	
  /* done with input, close */
  fclose(infile);

  /* write symbols to symfile */
  if(symout) {
    if ((symfile = fopen(symname, "w")) == NULL) {
      fprintf(stderr, "%s: Error opening symbol file %s.\n", argv[0], symname);
      exit(EXIT_FAILURE);
    }
    /* qsort works because symbol name is first in struct */
    qsort(symtab, symptr - symtab, sizeof(SYMTAB),
	  (int(*)(const void*, const void*))&strcmp);

    for (symtmp = symtab; symtmp != symptr; symtmp++) {		
      fprintf(symfile, "%s\t:\t %c ", symtmp->symbol, symflag[symtmp->type]);
      if(symtmp->type != macro) {
	fprintf(symfile, "%04o", symtmp->val.location);
      }
      fprintf(symfile, "\n");
    }
    fclose(symfile);
  }
		
  if (memtop == 0) {
    fprintf(stderr, "%s: Error. Input file generates no code.\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  if (errcnt > 0) {
    fprintf(stderr, "%d error(s). No code generated.\n", errcnt);
    exit(EXIT_FAILURE);
  }
  if ((outfile = fopen(outname, "w")) == NULL) {
    fprintf(stderr, "%s: Error opening output file %s.\n", argv[0], outname);
    exit(EXIT_FAILURE);
  }

  for (i = 0; i < memtop; i += 2) {
    l = i & MASK12;
    oc[0] = mem8[l] >> 4;
    oc[1] = ((mem8[l] & MASK4) << 4) | (mem8[l + 1] >> 8);
    oc[2] = mem8[l + 1] & MASK8;

    fwrite(oc, sizeof(unsigned char), 3, outfile);
  }
  fclose(outfile);

  if(verbose)
    printf("INFO: %s run finished.\n", prog);

  exit(EXIT_SUCCESS);
}
