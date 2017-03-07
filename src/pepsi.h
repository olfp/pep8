/*
   definitions for pep8 simulator - pepsi

   (c) olf 07/05
*/

#ifndef __PEPSI__
#define __PEPSI__

#include "global.h"

#define OPTCHR	'-'

#define ENVNAME	"PEPSI"

#define DOT	'.'

#define SRC	".pls"
#define IMG	".pmi"
#define COR	".pci"

#define SRCOFF	17			/* offset of sourceline in listfile */

#define OFFSEP  ','			/* offset delimiter for dump range */

/* prototypes */

void display();
void interact();
void terminal();
void interrupt();
void iotrap( WORD8 opcode );
void decodeopr( WORD8 opcode );
void execute( WORD8 opcode );

#endif

