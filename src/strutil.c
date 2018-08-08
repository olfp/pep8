/*
   utility functions
*/

#include <string.h>
#include <ctype.h>

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

char *strucpy(char *dst, char *src) {

	char *res = dst;
	for( ; *src; src++, dst++ ) {
		*dst = toupper(*src);
	}
	*dst = '\0';
	
	return res;
}


