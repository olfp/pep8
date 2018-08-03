/*
   pot - pep8 opcode translator

   (c) olf 07/05

   takes .pps file to produce an .pmi image file to be loaded into pepsi(1)

   missing features:
   1) . references current pc
   2) more pseudos
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>

#include "octal.h"
#include "pot.h"

WORD8 mem8[MEM8SIZ];		/* memory of pep8 */
WORD8 pc8;			/* current mem location */

int memtop;			/* highest touched mem location */

SYMTAB *symtab;			/* symbol table */
SYMTAB *symptr;			/* current symbol */
int symsiz = SYMSIZ;		/* size of symbol table */

int symout = FALSE;		/* write symbol table to file */
int lstout = FALSE;		/* write listing to file */

int lineno = 0;			/* current linenumber */
int errcnt = 0;			/* number of errors */

int run = TRUE;			/* reset if end of assemble char seen */

int textmode = 0;		/* assemble nums/sysm or text/sixbit */

extern int optind;
extern char *optarg;		/* argument pointer for getopt */

static void usage(char *name) {
  fprintf(stderr, "%s - pep8 opcode translator\n", name);
  fprintf(stderr, "usage: %s [-lsS<n>] <infile>[.pps]\n", name);
  fprintf(stderr, "  l - write a .pls listing file\n");
  fprintf(stderr, "  s - write symbols to a .psy file\n");
  fprintf(stderr, "  S - set size of symbol to <n>\n");
  exit(EXIT_FAILURE);
}

static void po_next_page() {

  pc8 &= ~MASK7;
  pc8 += PAGE8;
  if( pc8 > MASK12 ) {
    fprintf( stderr, "Fatal: page %o is out of range!\n", pc8 >> 7 );
    exit(EXIT_FAILURE);
  }
}

static void po_text() {
  textmode = 0xff;
}

static int pseudoop(char *tok) {

  PSEUDO *pop;
  
  for( pop = pseudos; *(pop->pseudo) != '\0'; pop++ ) {
    if( !strcmp( tok, pop->pseudo ) ) {
      (pop->pseudoop)();
      break;
    }
  }

  return( *(pop->pseudo) != '\0' );
}

static void symscan( char *line, FILE *symfile ) {

  char *tok, *nxt;
  int flag;
  int lblfld = TRUE;
  int cnt = 0;
  unsigned int ibuf;
  SYMTAB *tmpsym;

  tok = strtok( line, DELIMS );
  
  while( tok != NULL ) {

    nxt = tok + strlen(tok) + 1;
    
    /* check for end-of-assembly symbol */

    if( *tok == EOA ) {
      run = FALSE;		/* stop assembly */
      break;			/* exit processing this line */
    }

    /* check for comment */

    if( strchr(COMM, *tok) )
      break;			/* end processing this line */

    /* check for new target address */

    if( *tok == ORG ) {
      if( checkoctal(++tok, 4) ) {
	sscanf( tok, "%o", &ibuf);
	pc8 = (WORD8) ibuf;
      } else {
	fprintf( stderr, "%05d: bad octal number: %s\n", lineno, tok );
	errcnt++;
      }
      break;			/* done with this line */
    }

    if( lblfld && (strchr(LBLCHR, tok[strlen(tok)-1]) || strchr(tok, EQU))) {

      /* process labels and assignments */

      flag = (strchr(LBLCHR, tok[strlen(tok)-1]) != NULL);
      tok[strlen(tok) - 1] = '\0' ; /* eleminate the label delimiter */
      if( strlen(tok) > SYMLEN ) {
	fprintf( stderr, "%s: symbol too long. Truncated.\n", tok );
	tok[SYMLEN] = '\0';
      }

      /* check if symbol already exists */

      for( tmpsym = symtab; tmpsym != symptr; tmpsym++ ) {
	if( !strcmp( tmpsym->symbol, tok) ) {
	  fprintf( stderr, "Duplicate symbol %s.\n", tok );
	  errcnt++;
	  break;
	}
      }
      
      strncpy(symptr->symbol, tok, SYMLEN);
      (symptr->symbol)[SYMLEN] = '\0';
      if( flag ) {
	symptr->location = pc8;
      } else {
	assemble( nxt, NULL, &(symptr->location) );
      }

      if( symfile )
	fprintf( symfile, "%s\t:\t%04o\n", symptr->symbol, symptr->location );

      if( symptr >= symtab + symsiz ) {
	fprintf( stderr, "Fatal: symbol table overflow, line %05d!\n", lineno);
	exit(EXIT_FAILURE);
      }

      symptr++;
      *(symptr->symbol) = '\0';	/* mark end of table */

      if(!flag)                 /* value symbol */
	break;                  /* line is completely processed by 'assemble */

    } else {

      /* there is actual code in this line */
      if( pseudoop(tok) ) {	/* process pseudo operations */
	if(textmode){
	  tok += strlen(tok) + 1;
	  while(*tok && strchr(DELIMS, *tok)) {
	    tok++;
	  }
	  if(*tok) {
	    textmode = *tok++;
	    pc8++;		/* length */
	    cnt++;
	    while(*tok && (*tok != textmode)) {
	      pc8++;
	      cnt++;
	      tok++;
	      if(*tok && (*tok != textmode))
		tok++;
	    }
	  }
	  textmode = 0;
	} 
      } else {
	lblfld = FALSE;	/* none found, no more labels now */
      }
      break;			/* done with this line */
    }
    
    tok = strtok( NULL, DELIMS );
  }

  if( !lblfld ) {	/* if code was generated */
    ++pc8;
  }
}

static int valueof(char *tok, unsigned int *val) {

  OPCODE *code;
  SYMTAB *symptr;
  int type = TERROR;

  if( isdigit(*tok) || (strchr("+-", *tok) && isdigit(*(tok+1)))) { 
    /* its a number! */
    if( checkoctal(tok, 4) ) {
      sscanf( tok, "%o", val );
      type = TISVAL;
    } else {
      type = TISBAD;
    }
  } else {
    /* check for mnemonic or symbol */
    for( code = opcodes; *(code->mnemonic) != '\0'; code++ ) {
      if( !strcmp( tok, code->mnemonic ) ) {
	type = TMNEMO;
	*val = code->opcode;
	break;
      }
    }
    if( *(code->mnemonic) == '\0' ) { /* no mnemonic */
      /* must be a symbol */
      for( symptr = symtab; *(symptr->symbol) != '\0'; symptr++ ) {
	if( !strcmp( tok, symptr->symbol ) ) {
	  type = TISVAL;
	  *val = symptr->location;
	  break;
	}
      }
    }
  }
  
  return type;
}

static int assemble( char *line, FILE *lstfile, WORD8 *assembly ) {

  char *tok;
  WORD8 pos, op, loc = UNUSED;
  OPCODE *code;
  SYMTAB *symptr;
  char errstr[6] = "";
  char buf[BUFLEN];
  int lblfld = TRUE;
  int len, cnt = 0;
  unsigned int ibuf, newval, val; 
  int type, val6;
  char *close, *next;
  char prevop, opchar;

  strcpy( buf, line );	/* preserve input line for listing */

  tok = strtok( line, DELIMS );

  while( tok != NULL ) {

    /* check for end-of-assembly symbol */

    if( *tok == EOA ) {
      run = FALSE;		/* stop assembly */
      break;			/* exit processing this line */
    }

    /* check for new target address */

    if( *tok == ORG ) {
      if( checkoctal(++tok, 4) ) {
	sscanf( tok, "%o", &ibuf);
	pc8 = (WORD8) ibuf;
      } else {
	fprintf( stderr, "%05d: bad octal number: %s\n", lineno, tok );
	errcnt++;
	strcat( errstr, "N" );
      }
      break;			/* done with this line */
    }

    /* check for comment or assignment */

    if( strchr(COMM, *tok) || strchr(tok, EQU) )
      break;			/* end processing this line */

    if( lblfld && strchr( LBLCHR, tok[strlen(tok)-1] )) {

      /* process labels */

    } else {

      if( pseudoop(tok) ) {	/* process pseudo ops */
	if(textmode){
	  tok += strlen(tok) + 1;
	  while(*tok && strchr(DELIMS, *tok)) {
	    tok++;
	  }
	  if(*tok) {
	    textmode = *tok++;
	    len = strrchr(tok, textmode) - tok;
	    fprintf( lstfile, "%04o: %-5s %04o\t%s", pc8, errstr, len, buf );
	    mem8[pc8++] = len;
	    cnt++;
	    while(*tok && (*tok != textmode)) {
	      val6 = ((*tok - 32) << 6) | (*(tok+1) - 32);
	      if( lstfile )
		fprintf( lstfile, "%04o:       %04o\n", pc8, val6 );
	      mem8[pc8++] = val6;
	      cnt++;
	      tok++;
	      if(*tok && (*tok != textmode))
		tok++;
	    }
	  } else {
	    fprintf(stderr, "No text at TEXT pseudo!\n");
	  }
	  textmode = 0;
	}
	break;			/* rest of line is ignored */
      }

      /* process mnemonics & operands*/

      lblfld = FALSE;		/* no more labels now */

      if(*tok == STRTEX) {	/* arithmetic expression */
	type = TISBAD;		/* default to bad value */
	if( !( close = strchr( tok, ENDEX ))) {
	  fprintf(stderr,"%05d: unterminated expression %s\n", lineno, tok);
	  errcnt++;
	  strcat( errstr, "U" );
	} else {
	  *close = '\0';
	  *tok++;
	  val = 0;
	  prevop = '+';
	  while( *tok ) {
	    next = tok;
	    if( strchr( ARITHC, *next ))
	      next++;		/* skip leading signs */
	    while( *next && !strchr( ARITHC, *next )) {
	      next++;
	    }
	    opchar = *next;
	    *next++ = '\0';
	    valueof(tok, &newval);
	    switch(prevop) {
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
	    }
	    prevop = opchar;
	    tok = next;
	  }
	  type = TISVAL;
	}
      } else {
	type = valueof(tok, &val);
      }

      if(type == TMNEMO) {	/* its an opcode */
	if( loc & UNUSED ) 
	  loc = val;
	else
	  loc |= val;
      }
      if(type == TISVAL) {	/* its a valid number or symbol */
	op = (WORD8) val;
	if( loc & UNUSED ) {
	  loc = op;
	} else {
	  if( (op & PAGEMSK) ) { /* in-page address? */
	    if( (op & PAGEMSK) != (pc8 & PAGEMSK) ) { /* not this page? */
	      fprintf(stderr,"%05d: off page reference %s\n", lineno, tok);
	      errcnt++;
	      strcat( errstr, "P" );
	    }
	    loc |= INPAGE;	
	  }
	  loc |= (op & ADDRMSK);
	}
      } 
      if(type == TISBAD) {	/* its an invalid number */
	fprintf( stderr, "%05d: bad octal number: %s\n", lineno, tok );
	errcnt++;
	strcat( errstr, "N" );
      }
      if(type == TERROR) {	/* its a nothing, probably an undef sym */
	fprintf( stderr, "%05d: undefined symbol: %s\n", lineno, tok );
	errcnt++;
	strcat( errstr, "S" );
      }
    }
    
    tok = strtok( NULL, DELIMS );
  }

  loc &= MASK12;

  if( lstfile && !cnt)
    fprintf( lstfile, "%04o: %-5s %04o\t%s", pc8, errstr, loc, buf );

  if(assembly) 		/* return generated bits if requested */
    *assembly = loc;

  if(!lblfld && !cnt && !assembly) /* did code, no textmode, no sympass */
    mem8[pc8++] = loc;

  return( !lblfld );	/* return true if code was generated */
}

int main( int argc, char *argv[] ) {

  FILE *infile, *outfile, *lstfile, *symfile;
  char *inname, *outname, *lstname, *symname;
  char linebuf[BUFLEN];
  char *prog, *file;
  char optch;
  int i, j, f, l, len;
  unsigned char oc[3];

  prog = argv[0];
  if(!(prog = strrchr(prog, '/'))) {
    prog = argv[0];
  } else {
    prog++;
  }
  
  while( (optch = getopt( argc, argv, "lsS:?" )) > 0 ) {
    switch( optch ) {
    case 'l':			/* write listing file */
      lstout = TRUE;
      break;
    case 's':			/* write symbol table to file */
      symout = TRUE;
      break;
    case 'S':			/* Symbol table size */
      symsiz = atoi(optarg);
      break;
    default:
      usage(prog);
      break;
    }
  }

  if( optind >= argc ) {
    usage(prog);
  }

  /* process filename given */

  file = argv[optind];

  if( strchr( file, DOT )) {
    len = strlen(file) + 4;	/* just to be sure take a few bytes more */
    inname = (char *) malloc( len );
    strcpy( inname, file );
  } else {
    len = strlen(file) + strlen(SRC) + 1;
    inname = (char *) malloc(len);
    strcpy( inname, file );
    strcat( inname, SRC );
  }

  outname = (char *) malloc( len );
  strcpy( outname, inname );
  strcpy( strrchr(outname, DOT ), DST );

  if( lstout ) {
    lstname = (char *) malloc( len );
    strcpy( lstname, inname );
    strcpy( strrchr(lstname, DOT ), LST );
  }

  if( symout ) {
    symname = (char *) malloc( len );
    strcpy( symname, inname );
    strcpy( strrchr(symname, DOT ), SYM );
  }

  /* open input file */

  if( (infile = fopen( inname, "r" )) == NULL ) {
    fprintf( stderr, "%s: File %s not found.\n", argv[0], inname );
    exit(EXIT_FAILURE);
  }

  /* build symbol table */

  symptr = symtab = (SYMTAB *) malloc( symsiz * sizeof(SYMTAB));
  *(symptr->symbol) = '\0';	/* mark end of table */

  if( symout ) {
    if( (symfile =  fopen( symname, "w" )) == NULL ) {
      fprintf(stderr,"%s: Error opening symbol file %s.\n", argv[0], symname );
      exit(EXIT_FAILURE);
    }
  } else {
    symfile = NULL;		/* to prevent output in symscan() */
  }

  memtop = pc8 = 0;		/* start at loc zero by default */

  while( run && fgets( linebuf, BUFLEN, infile )) {
    for( i = 0; i < strlen(linebuf); i++ ) {
      linebuf[i] = toupper(linebuf[i]);
    }
    lineno++;
    symscan( linebuf, symfile );
  }

  if( symout )
    fclose(symfile);

  if( errcnt > 0 ) {
    fprintf( stderr, "%d error(s) after pass 1. Abort.\n", errcnt );
    exit(EXIT_FAILURE);
  }
  
  /* initialize mem buffer / loc pointer */

  for( j = 0; j < MEM8SIZ; j++ )
    mem8[j] = 0;		/* zap memory buffer */

  memtop = pc8 = 0;		/* start at loc zero by default */

  /* rewind input file */

  rewind(infile);
  lineno = 0;

  /* do the assembly */

  if( lstout ) {
    if( (lstfile = fopen( lstname, "w" )) == NULL ) {
      fprintf( stderr, "%s: Error opening list file %s.\n", argv[0], lstname );
      exit(EXIT_FAILURE);
    }
  } else {
    lstfile = NULL;		/* to preven output in assemble() */
  }

  run = TRUE;

  while( run && fgets( linebuf, BUFLEN, infile )) {
    for( i = 0; i < strlen(linebuf); i++ ) {
      linebuf[i] = toupper(linebuf[i]);
    }
    lineno++;
    if( assemble( linebuf, lstfile, NULL)) {
      if( pc8 > memtop )
	memtop = pc8;		/* keep track of user mem */
    }
  }

  if( lstout ) {
    fputc( '\n', lstfile );
    fclose(lstfile);
  }
  
  fclose(infile);

  if( memtop == 0 ) {
    fprintf( stderr, "%s: Error. Input file generates no code.\n", argv[0] );
    exit(EXIT_FAILURE);
  }

  if( errcnt > 0 ) {
    fprintf( stderr, "%d error(s). No code generated.\n", errcnt );
    exit(EXIT_FAILURE);
  }
  
  if( (outfile = fopen( outname, "w" )) == NULL ) {
    fprintf( stderr, "%s: Error opening output file %s.\n", argv[0], outname);
    exit(EXIT_FAILURE);
  }

  for( i = 0; i < memtop; i += 2 ) {
    l = i & MASK12;
    oc[0] = mem8[l] >> 4;
    oc[1] = ((mem8[l] & MASK4) << 4) | (mem8[l+1] >> 8);
    oc[2] = mem8[l+1] & MASK8;
    
    fwrite( oc, sizeof(unsigned char), 3, outfile );
  }
  
  fclose( outfile );

  exit(EXIT_SUCCESS);
}

