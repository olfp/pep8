/*
    pdp8 project

    functions dealing with octal numbers
*/

#include <string.h>
#include "types8.h"

int checkoctal( char *tok, int n ) {

  int i, ok = !0;

  if(!tok)				/* null pointer */
    return 0;

  if( *tok == '-' )                     
    tok++;                              /* allow negative numbers */

  if( n && strlen(tok) > n )
    return 0;				/* only n octal digits */

  for( i = 0; tok[i]; i++ ) {
    if( !isoctal(tok[i]) ) {
      ok = 0;
      break;
    }
  }

  return ok;
}




