/*
   defs for utility functions
*/

#ifndef __UTIL__
#define __UTIL__

#include "global.h"

void dumpmem( WORD8 addr, int cnt );
char *getparam( char * cmd, int * addr, char **sym, int discard);

#ifdef GNURL
char **sym_completion(const char *text, int start, int end);
#endif

char *util_readline(char *prompt);

#endif
