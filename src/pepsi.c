/*
   pep8 simulator

   (c) olf 07/05, 2018

   takes .pmi file and executes it

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

#ifdef GNURL
#include <readline/readline.h>
#endif

#include "strutil.h"
#include "util.h"
#include "chario.h"
#include "octal.h"
#include "pepsi.h"

extern int errno;
extern int optind;
extern char *optarg;

WORD8 mem8[MEM8SIZ];			/* memory of pdp8 */
WORD8 pc8 = 0;				/* current mem location, default 0 */
WORD8 ac8;				/* accumulator */
BIT8  link8;			        /* link bit */
BIT8  ien8 = 0;				/* interrupt enable */
BIT8  irq8 = 0;				/* interrupt request */

int memtop;				/* highest touched mem location */

int running = TRUE;			/* run or halt */
int interactive = FALSE;		/* interactive mode */
int metrics = FALSE;			/* output metrics */
int singlestep = FALSE;			/* pause after each instruction */
int verbose = FALSE;			/* print trace while processing */
int usesource = FALSE;			/* whether to use the source */
int coredump = FALSE;			/* whether to produce a coredump */
int readfile = TRUE;			/* whether to read an image file */

char **text = NULL;			/* source text */
int maxtxt = 0;				/* addr of max. text line avail. */

int iobase = IOBASE_DEFAULT;		/* tcp port base for dev io */

char *devices[MAXDEV];		        /* pointer to dev parame */

DUMP *dumps = NULL;                     /* dump list */
WATCH *watched = NULL;			/* watch list */
SYMBOL *symbols = NULL;			/* symbol table */

char *origin = NULL;			/* origin sym name or octal number */

/* breakpoint list */

BRKPNT *brkfirst = NULL;		/* first breakpoint */
BRKPNT *brklast = NULL;			/* last breakpoint */

int submod = 0;				/* breakpoint offset for jms */

/* timings */
struct timeval  tstart, tfinish;

static void usage( char *name ) {

  fprintf(stderr ,"%s - pep8 simulator.\n", name);
  fprintf(stderr ,"usage: %s [-vstim] [-o addr] [-d addr,cnt ...] [-e d0,d1 ...] <image>.[pmi]\n", name);
  fprintf(stderr, "  v - verbose: print status information while running\n");
  fprintf(stderr, "  s - source: use assembler source listing for display\n");
  fprintf(stderr, "  t - trace: enter interactive trace mode\n");
  fprintf(stderr, "  i - interactive: combies v, s and t\n");
  fprintf(stderr, "  m - metrics: output runtime and ips\n");
  fprintf(stderr, "  o - set origin of program counter\n");
  fprintf(stderr, "  d - dump cnt words at addr after HLT instruction\n");
  fprintf(stderr, "  e - enable listed devices\n");
  exit(EXIT_FAILURE);
}

#ifdef GNURL

static void init_rl( char *progname ) {
  rl_readline_name = progname;   
#ifndef MACRL
  rl_attempted_completion_function = sym_completion;
#endif
}

#endif

int main( int argc, char *argv[] ) {

  FILE *infile, *corefile, *srcfile;
  char *inname, *corename, *srcname;
  char linebuf[BUFLEN], last[BUFLEN], lblbuf[BUFLEN];
  char *prog, *file, *daddr, *dcnt, *next;
  int i, j, isopt, len, dev;
  int addr;
  unsigned char oc[3];
  int optch;
  char *txt, *lbl;
  struct stat stbuf;
  DUMP *newdump;
  BRKPNT *brk;
  SYMBOL *lastsym, *newsym, *sym;
  /* metrics */
  struct timeval  tstart, tfinish;
  double runtime, ips, mips;
  char *magpfx = "";
  unsigned long icnt8 = 0;

  prog = argv[0];
  if(!(prog = strrchr(prog, '/'))) {
    prog = argv[0];
  } else {
    prog++;
  }

  for(i = 0; i < MAXDEV; i++)
    devices[i] = NULL;
  
  while( (optch = getopt( argc, argv, "vstimo:d:p:e:?" )) > 0 ) {
    switch( (char)optch ) {
    case 'v':			/* verbose */
      verbose = TRUE;
      break;
    case 's':			/* use source */
      usesource = TRUE;
      break;
    case 'i':			/* interactive */
      interactive = TRUE;
      usesource = TRUE;
      verbose = TRUE;
      /* fallthru */
    case 't':			/* trace */
      singlestep = TRUE;
      break;
    case 'm':			/* metrics */
      metrics = TRUE;
      break;
    case 'o':			/* set origin */
/*
      origin = (char *)strdup(optarg);
*/
      origin = optarg;
      break;
    case 'e':			/* enable devices */
      do {
	if((next = strchr(optarg, ARGSEP))) { /* extra parens for gcc */
	  *(next++) = '\0';
	}
	if(isdigit(*optarg)) {
	  dev = *optarg++ - '0';
	  if(!*optarg) {
	    devices[dev] = "";
	  } else if(*optarg == DEVSEP) {
/*
	    devices[dev] = (char *)strdup(++optarg);
*/
	    devices[dev] = ++optarg;
	  } else {
	    fprintf(stderr, "WARNING: Bad device spec: %s\n", optarg);
	  }
	} else {
	  fprintf(stderr, "WARNING: No device spec: %s\n", optarg);
	}
      } while((optarg = next)); /* extra parens for gcc */
      break;
    case 'd':			/* coredump */
      coredump = TRUE;
      daddr = optarg; /* (char *)strdup(optarg); */
      dcnt = strchr(daddr, OFFSEP);
      if(dcnt) {
	*dcnt = '\0';
	dcnt++;
      }
      if(dcnt && !checkoctal(dcnt, 0)) {
	fprintf(stderr, "Warning: bad dump range %s, count must be octal.\n",
		optarg);
      } else {
	newdump = (DUMP *) malloc( sizeof(DUMP));
	newdump->from = upstr(daddr);
	if(dcnt)
	  sscanf( dcnt, "%o", &(newdump->cnt));
	else
	  newdump->cnt = 1;
	newdump->next = dumps;
	dumps = newdump;
      }
      break;
    default:			/* report usage */
      usage(prog);
      break;
    }
  }
  
  if( optind >= argc ) {
    usage(prog);
  }

  if( optind >= argc ) {
    usesource = FALSE;
    singlestep = TRUE;
    coredump = FALSE;
    readfile = FALSE;
  }

#ifdef GNURL
  
  /* initialize readline lib */

  init_rl( argv[0] );

#endif

  if( readfile ) {

    /* process filename given */

    file = argv[optind];

    if( strchr( file, DOT )) {
      len = strlen(file) + 4;	/* give a little more space */
      inname = malloc( len );
      strcpy( inname, file );
    } else {
      len = strlen(file) + strlen(IMG) + 1;
      inname = malloc(len);
      strcpy( inname, file );
      strcat( inname, IMG );
    }

    corename = malloc( len );
    strcpy( corename, inname );
    strcpy( strrchr(corename, DOT ), COR );
    
    srcname = malloc( len );
    strcpy( srcname, inname );
    strcpy( strrchr(srcname, DOT ), SRC );

    /* get size of image file */

    if( stat(inname, &stbuf) == -1)  {
      fprintf( stderr, "Fatal: cant stat file %s.\n", inname );
      exit(EXIT_FAILURE);
    }

    if( S_ISDIR(stbuf.st_mode)) {
      fprintf( stderr, "Fatal: %s is a directory!\n", inname );
      exit(EXIT_FAILURE);
    }

    len = stbuf.st_size * 2 / 3;		/* size in 12 bit words */
    if( len > MEM8SIZ ) {
      fprintf(stderr,"Fatal: image file %s larger then %d words!\n",
	      inname, MEM8SIZ);
      exit(EXIT_FAILURE);
    }

    if( usesource ) {
		maxtxt = len; /*  - 1; */

		/* open source file */
      
		if( (srcfile = fopen( srcname, "r" )) == NULL ) {
			fprintf( stderr, "%s: Listfile %s not found.\n", argv[0], srcname );
			exit(EXIT_FAILURE);
		}

		text = (char **) malloc( maxtxt * sizeof(char **));
        
		for( i = 0; i < maxtxt; i++ )
			text[i] = NULL;		/* make sure no junk is in there */
    
		i = -1;				/* current location */

		/* read source lines */

		while( fgets( linebuf, BUFLEN, srcfile )) {
			upstr(linebuf);

			sscanf( linebuf, "%o:", (unsigned *)&addr );
			txt = last + SRCOFF;
			if( addr != i ) {
				if(i < 0) 				/* first line, zero loc */
					i = 0;
				text[i] = malloc( strlen(txt) + 1 );
				strcpy( text[i], txt );
				i = addr;
			}
			txt = strcpy( lblbuf, linebuf) + SRCOFF;
			if( strchr(COMM, *txt) )
				*txt = '\0';				/* whole line comment */
			else
				txt = strtok( txt, COMM );		/* strip of comment */
			lbl = txt = unspace(txt);		/* strip of whitespace */
			while( txt && *txt && !strchr(LBLCHR, *txt) && !strchr(TOKDEL, *txt)) {
				txt++;
			}
			if( txt && *txt && strchr(LBLCHR, *txt)) {
				*txt = '\0'; 				/* strip of LBLCHR */
				newsym = (SYMBOL *) malloc( sizeof(SYMBOL));
				newsym->location = addr;
				strcpy(newsym->symbol, lbl);
				newsym->next = NULL;
				if( symbols == NULL ) {
					symbols = lastsym = newsym;
				} else {
					lastsym->next = newsym;
					lastsym = newsym;
				}
			}
			strcpy( last, linebuf );  
		}

		text[i] = malloc( strlen(last + SRCOFF) + 1 );
		strcpy( text[i], last + SRCOFF );
	  
		fclose( srcfile );
    }

    /* open input file */

    if( (infile = fopen( inname, "r" )) == NULL ) {
		fprintf( stderr, "%s: File %s not found.\n", argv[0], inname );
		exit(EXIT_FAILURE);
    }

    for( addr = 0, i = 0; i < len; i += 2, addr += 2 ) {

		fread( oc, sizeof(unsigned char), 3, infile );
      
		mem8[addr]   = (oc[0] << 4) | (oc[1] >> 4);
		mem8[addr+1] = ((oc[1] & MASK4) << 8) | oc[2];
    }
  
    fclose( infile );

    memtop = len;
  }

  /* code is in memory, go execute it */

  /* figure out where to start */

  if(origin) {
    getparam( origin, (int *)&pc8, NULL, 0);
    if( pc8 < 0 ) {
      fprintf(stderr, "Fatal: bad origin %s, neither symbol nor octal.\n", 
	      optarg);
      exit(EXIT_FAILURE);
    }
  } 

  /* set up io devices */

  chario_init(iobase, devices);

  /* stamp start time */
  
  gettimeofday(&tstart, NULL);

  /* go ahead */

  while( running ) {
    if( singlestep || verbose ) {
      display();
    }
    if( singlestep ) {
      interact();
    }
    execute( mem8[pc8] );
    icnt8++;
    
    if( pc8 > memtop )
      memtop = pc8;

    /* check for interrupt enable and interrut request */

    if( ien8 & irq8 ) {
      ien8 = 0;			/* disable interrupts */
      irq8 = 0;			/* flag as serviced */
      mem8[0] = pc8;		/* save pc in loc 0 */
      pc8 = 00001;		/* jump to loc 1 */
      if(verbose)
	printf("INTERRUPT!\n");
    }
    
    if( brkfirst ) {
      for( brk = brkfirst; brk != NULL; brk = brk->next ) {
	if( (brk->addr == pc8) || ( brk->addr == (pc8 - submod)) ) {
	  singlestep = TRUE;
	  verbose = TRUE;
	  break;
	}
      }
    }

    if( !running && interactive ) {
      running = TRUE;
      singlestep = TRUE;
      verbose = TRUE;
    }
  }

  /* stamp stop time */
  
  gettimeofday(&tfinish, NULL);
  runtime = (double) (tfinish.tv_usec - tstart.tv_usec) / 1000000 +
    (double) (tfinish.tv_sec - tstart.tv_sec);
  ips = (double)icnt8 / runtime;
  mips = ips / 1000000;

  if(metrics)
    printf("Processed %ld instructions in %lf seconds, %.2lf MIPS.\n",
	   icnt8, runtime, mips);

  /* close io devices */

  chario_close();

  /* dump mem if requested */

  if( coredump ) {
    while( dumps ) {

      getparam( dumps->from, &addr, NULL, 0);
      if(addr < 0) {
		fprintf(stderr, "Warning: cannot dump from %s, %s.\n", dumps->from,
			(symbols ? "neither symbol nor octal" : "no symbols loaded"));
      } else {
		if(!checkoctal(dumps->from, 0))
			printf("%s:\n", dumps->from);
			dumpmem( addr, dumps->cnt );
		}
      
		dumps = dumps->next;
    }
  }

  exit(EXIT_SUCCESS);
}

