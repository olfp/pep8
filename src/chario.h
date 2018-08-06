/*
  pep8 simulator

  character i/o includes
*/

#ifndef __CHARIO__
#define __CHARIO__

#define MAXDEV 16			/* we have four device bits */

static int devstat[MAXDEV];

void chario_init(int iobase, char *devdesc[]);
void chario_close();
int chario(int iop, int device, WORD8 *ac);

#endif
