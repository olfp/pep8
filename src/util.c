/*
   utility functions
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#ifdef GNURL
#include <readline/readline.h>
#endif

#include "octal.h"
#include "global.h"

#define DMPLEN	12		/* words per line while dumping mem */

static struct termios tbufsave;

char *upstr( char *s ) {

  char *p;

  for( p = s; *p; p++ ) {
    *p = toupper(*p);
  }

  return(s);
}

char *unspace( char *s ) {

  char *p, *t;

  if( !s )
    return(s);

  for( p = s; isspace(*p); p++ )
    ;

  if( *p == '\0' )
    return(p);

  t = p + strlen(p) - 1;
  while(t > p && isspace(*t))
    t--;
  *++t = '\0';

  return(p);
}

char *getparam( char * cmd, int * addr, char **symp, int discard ) {

  char *tok, *num, *next, *tmp;
  int page;
  SYMBOL *sym;
  
  /* if told, skip cmd char, check arg */
  
  if(discard) { 
    cmd += discard;
    if(*cmd != ' ' ) {
      *addr = (*cmd == '\0') ? NOPARA : ADRERR;
    }
  }

  /* start tokenize, get first token */

  tok = strtok_r( cmd, TOKDEL, &next );

  if( !tok ) {
    *addr = NOPARA;			/* no arg given */
    return next;
  } else {

    /* convert to uppercase */

    upstr(tok);

    /* check if its a symbol */
    if(symp)
      *symp = NULL;

    for( sym = symbols; sym != NULL; sym = sym->next ) {
      if( !strcmp(sym->symbol, tok)) {
        *addr = sym->location;
        if(symp)
          *symp = sym->symbol;
        return next;
      }
    }
  
    /* look if its an absolute address or in page:offset form */
  
    num = strdup(tok);
    if((tmp = strchr(num, PAGSEP))) { /* extra parens for gcc */
      *tmp = '\0';
      tmp++;
      if( !(checkoctal(tok, 0) && checkoctal(tmp, 0))) {
        *addr = ADRERR;
        return next;
      } else {	    
        sscanf( tok, "%o", (unsigned *)&page );
        sscanf( tmp, "%o", (unsigned *)addr );
        if( (page > MAXPAG) || (page < 0)) {
          *addr = ADRERR;
          return next;
        } else if( (*addr > MASK7) || (*addr < 0)) {
          *addr = ADRERR;
          return next;
        } else {
          *addr = (page << 7) + *addr;
        }
      }
    } else {
      if( !checkoctal(tok, 4) ) {
        *addr = ADRERR;
        return next;
      } else {
          sscanf( tok, "%o", (unsigned *)addr );
      }
    }
  }

  return next;
}

void dumpmem( WORD8 addr, int cnt) {

  int i;

  for(i = 0; i < cnt; i++) {
    if(!(i % DMPLEN)) {
      if(i)
		putchar('\n');

      printf("%04o: ", addr + i);
    }

    printf("%04o ", mem8[addr + i]);
  }
  putchar('\n');
}

#if defined(GNURL) && !defined(MACRL)

/* this is shamelessly stolen from the GNU readline info doc */

char *sym_generator(const char *text, int state) {

  static int len;
  static SYMBOL *sym;
  char *name;

  /* If this is a new word to complete, initialize now.  This
     includes saving the length of TEXT for efficiency, and
     initializing the index variable to 0. */
  if (!state) {
    sym = symbols;
    len = strlen (text);
  }

  /* Return the next name which partially matches from the symbol table. */

  while( sym ) {
    name = sym->symbol;
    sym = sym->next;
    if( !strncasecmp(name, text, len)) { /* match found */
      return (char *)strdup(name);
    }
  }
  
  return (char *)NULL;
}

char **sym_completion(const char *text, int start, int end) {

  char **match = (char **) NULL;

  if (start != 0)		/* no cmd, might be a symbol */
    match = rl_completion_matches( text, sym_generator );
  else
    rl_attempted_completion_over = 1; /* dont do filenames */

  return(match);
}

#else

#define INBUFLEN 100

void tio_save() {
    if (tcgetattr(STDIN_FILENO, &tbufsave) == -1)
        perror("tio_save/get");
}

void tio_raw() {

  struct termios tbuf;

    if (tcgetattr(STDIN_FILENO, &tbuf) == -1)
        perror("tio_raw/read");
  tbuf.c_iflag &= ~(INLCR | ICRNL | ISTRIP);
  tbuf.c_lflag &= ~(ICANON | ECHO);
  tbuf.c_cc[VMIN] = 1;
  tbuf.c_cc[VTIME] = 0;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tbuf) == -1)
        perror("tio_raw/set");
}

void tio_restore() {
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tbufsave) == -1)
        perror("tio_restore/set");
}

char *util_readline( char *prompt ) {

  char ch, *buf;
  int n, len, i = 0;

  len = INBUFLEN;
  buf = (char *) malloc( INBUFLEN );
  printf("%s", prompt);
  fflush(stdout);

  tio_save();
  tio_raw();

  while( ch != '\r' ) {
    read( STDIN_FILENO, &ch, 1);
    if( ch != '\b' )
      if( i < (len - 1)) {
	putchar(ch);
	buf[i++] = ch;
      } else
	putchar('\a');
    else
      if( i > 0 ) {
	buf[--i] = '\0';
	fputs("\b \b", stdout);
      } else 
	putchar('\a');
    fflush(stdout);
  }

  buf[i-1] = '\0';
  putchar('\n');

  tio_restore();

  return( buf );
}

#endif
