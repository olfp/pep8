/*
   defs for utility functions
*/

#ifndef __UTIL__
#define __UTIL__

void tio_save();
void tio_raw();
void tio_restore();

char *unspace( char *s );
char *upstr( char *s );

#ifndef GNURL
char *readline(char *prompt);
#endif

#endif
