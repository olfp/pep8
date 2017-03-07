/*
    pep8 simulator

    user interaction
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef GNURL
#include <readline/readline.h>
#include <readline/history.h>
#endif

#include "global.h"
#include "octal.h"
#include "util.h"

#include "interact.h"

static void usage() {
  printf("<cr> to execute next instruction\n\n");
  printf(".              - redisplay current trace info\n");
  printf("b <addr>       - set breakpoint at address <addr>\n");
  printf("bc <addr>      - clear breakpoint at address <addr>\n");
  printf("bl             - list breakpoints\n");
  printf("c[odh] <num>   - convert octal/decimal/hex number <num>\n");
  printf("d <addr> <val> - deposit value <val> at address <addr>\n");
  printf("g              - go continuous from pc (upto next breakpoint)\n");
  printf("o              - set new origin of pc to <addr>\n");
  printf("p              - print the next ten source lines\n");
  printf("p <addr>       - print ten source lines starting at <addr>\n");
  printf("p <addr>,<cnt> - print <cnt> source lines starting at <addr>\n");
  printf("s              - print the symbol table\n");
  printf("w <addr>       - watch word at address <addr>\n");
  printf("w <addr>,<cnt> - watch <cnt> words at address <addr>\n");
  printf("wc             - clear watch list\n");
  printf("x <addr>       - examine word at address <addr>\n");
  printf("x <addr>,<cnt> - examine <cnt> words at address <addr>\n");
  printf("q              - quit the simulator\n\n");
  printf("Addresses must be symbols or octal numbers up to four digits.\n\n");
}

static void showsrc(WORD8 addr, int cnt) {

  int i;
  char *src;

  if( addr <= maxtxt ) {
    for( i = 0; ( i < cnt ) && ((i + addr) <= maxtxt); i++ ) {
      printf( "%04o: %04o", addr+i, mem8[addr+i]);
      src = text[addr + i];
      if( src && *src)
	printf("\t%s", src);
      else
	putchar('\n');
    }
  } else {
    printf("no source line available.\n");
  }
}

static void deposit( char *cmd ) {

  char *tok;
  int addr, val;

  cmd = getparam( cmd, &addr, NULL, 1);
  if( addr >= 0 ) {
    cmd = getparam( cmd, &val, NULL, 0);
    if(val >= 0) {
      mem8[addr] = val & MASK12;
    } else {
      printf(NOCTVAL);
    }
  } else {
    printf(BADMEMA);
  }
}

static void examine( char *cmd ) {

  int addr, cnt;

  cmd = getparam( cmd, &addr, NULL, 1);
  if( addr >= 0 ) {
    cmd = getparam( cmd, &cnt, NULL, 0);
    if( cnt < 0 ) 
      cnt = 1;
    dumpmem( addr, cnt );
  } else {
    printf(BADMEMA);
  }
}

static void clearwatch( ) {

  WATCH *this;

  while(watched) {
    this = watched;
    watched = watched->next;
    free(this);
  }
}

static void watch( char *cmd ) {

  int addr, cnt;
  char *sym;
  WATCH *newwatch;

  cmd = getparam( cmd, &addr, &sym, 1);
  if( addr >= 0 ) {
    cmd = getparam( cmd, &cnt, NULL, 0);
    if( cnt < 0 ) 
      cnt = 1;
    
    newwatch = (WATCH *) malloc( sizeof(WATCH));
    newwatch->sym = sym;
    newwatch->addr = addr;
    newwatch->cnt = cnt;
    newwatch->next = watched;
    watched = newwatch;
  } else {
    printf(BADMEMA);
  }
}

static void setorigin( char *cmd ) {

  int addr;

  cmd = getparam( cmd, &addr, NULL, 1);
  if( addr >= 0 ) {
    pc8 = (WORD8) addr;
    display();
  } else {
    printf(BADMEMA);
  }
}

static void printsrc( char *cmd ) {

  int addr, cnt;

  if( !text ) {
    printf("no source available!\n");
  } else {
    cmd = getparam( cmd, &addr, NULL, 1);
    if( addr == NOPARA ) {
      addr = pc8;
      cnt = NUMLN;
    } else if( addr != ADRERR ) {
      cmd = getparam( cmd, &cnt, NULL, 0);
      if(cnt == NOPARA) {
	cnt = NUMLN;
      }
    }
    if((addr != ADRERR) && (cnt != ADRERR)) {
      showsrc( addr, cnt );
    } else {
      printf(BADMEMA);
    }
  }
}

static void setbreak( char *cmd ) {

  int addr;
  BRKPNT *brk;

  cmd = getparam( cmd, &addr, NULL, 1);
  if( addr >= 0 ) {
    if( !brkfirst ) {
      brklast = brkfirst = (BRKPNT *) malloc( sizeof(BRKPNT));
    } else {
      brk = (BRKPNT *) malloc( sizeof(BRKPNT));
      brklast->next = brk;
      brklast = brk;
    }
    brklast->addr = addr;
    brklast->next = NULL;
  } else {
    printf(BADMEMA);
  }
}

static void clearbreak( char *cmd ) {

  int addr;
  BRKPNT *brk, *prev;

  cmd = getparam( cmd, &addr, NULL, 2);
  if( addr >= 0 ) {
    if( !brkfirst ) {
      printf(BADBRKA);
    } else {
      for( brk = brkfirst; brk != NULL; brk = brk->next ) {
	if( brk->addr == addr ) {
	  if( (brk == brkfirst) && (brk == brklast) )
	    brkfirst = brklast = NULL;
	  else if( brk == brkfirst )
	    brkfirst = brkfirst->next;
	  else
	    prev->next = brk->next;

	  break;
	}
	prev = brk;
      }
      if( brk )
	free( brk );
      else
	printf(BADBRKA);
    }
  } else {
    printf(BADMEMA);
  }
}

static void listbreak(char *cmd) {

  int addr;
  BRKPNT *brk;

  if(!strchr(" \0", *(cmd+2))) {
    printf(ILLEGAL);
  } else if( brkfirst ) {
    for( brk = brkfirst; brk != NULL; brk = brk->next ) {
      addr = brk->addr;
      printf("%04o: %04o", addr, mem8[addr] );
      if((addr <= maxtxt) &&
	 	text[addr] && (text[addr][0] != '\n')) {
      printf( "\t%s", text[addr] );
    } else {
      putchar('\n');
    }

    }
  } else {
    printf("no breakpoints\n");
  }
}

static void printsym(char *cmd) {

  SYMBOL *sym;

  if(!strchr(" \0", *(cmd+1))) {
    printf(ILLEGAL);
  } else if( symbols ) {
    for( sym = symbols; sym != NULL; sym = sym->next ) {
      printf("%-7s: %04o\n", sym->symbol, sym->location );
    }
  } else {
    printf("no symbols\n");
  }
}
  
static void convert( char *cmd ) {

  int num;
 
  if( *(cmd + 2) != ' ' ) {
    printf(ILLEGAL);
  } else {
    switch( *(cmd + 1)) {
    case 'o':                           /* convert from octal */
      sscanf( cmd + 3, "%o", &num );
      printf("dez:%04d  hex:%03x\n", num, num );
      break;
    case 'h':                           /* convert from hex */
      sscanf( cmd + 3, "%x", &num );
      printf("oct:%04o  dez:%04d\n", num, num );
      break;
    case 'd':                           /* convert from decimal */
      sscanf( cmd + 3, "%d", &num );
      printf("oct:%04o  hex:%03x\n", num, num );
      break;	
    default:    
      printf("Unknown radix. Cannot convert.\n");
      break;
    }
  }
}

void interact() {

  char *cmd, *line = NULL;
  int done = FALSE;

  while( !done ) {

    if(line)
      free(line);

#ifdef GNURL
    line = readline(PROMPT);
    cmd = unspace(line);
    
    if( cmd && *cmd )
      add_history(cmd);
#else    
    line = util_readline(PROMPT);
    cmd = unspace(line);
#endif    

    if( cmd ) {
      switch( *cmd ) {
      case '\0':
        done = TRUE;			/* just return -> next intruction */
        break;
      case 'g':
        singlestep = FALSE;		/* go to next breakpoint */
        verbose = FALSE;
        done = TRUE;
        break;
      case 'q':
        running = FALSE;		/* quit the simulator */
	interactive = FALSE;
        done = TRUE;
        break;
      case '.':
        display();			/* re-display trace information */
        break;
      case 'b':
        switch(*(cmd + 1)) {
        case ' ':
  	  setbreak(cmd);		/* set breakpoint */
  	  break;
        case 'c':
  	  clearbreak(cmd);		/* clear breakpoint */
	  break;
        case 'l':
	  listbreak(cmd);		/* list breakpoints */
	  break;
        default:
	  printf(ILLEGAL);
	  break;
        }
        break;
      case 'c':
        convert(cmd);
        break;
      case 'd':
        deposit(cmd);
        break;
      case 'o':
        setorigin(cmd);
        break;
      case 'p':
        printsrc(cmd);
        break;
      case 's':
	printsym(cmd);
	break;
      case 'w':
        switch(*(cmd + 1)) {
        case 'c':
  	  clearwatch();			/* clear watch list */
	  break;
        default:
	  watch(cmd);
	  break;
        }
        break;
      case 'x':
        examine(cmd);
        break;
      case '?':
        usage();
        break;
      default:
        printf(ILLEGAL);
        break;
      }
    } else {
      putchar('\n');
      running = FALSE;		/* EOF, quit the simulator */
      done = TRUE;
    }
  }
}

