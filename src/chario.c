/*
  pep8 simulator

  character i/o 
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#if 0
#include <stropts.h>
#endif
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "global.h"
#include "chario.h"

#define IOP_RDYIN	0
#define IOP_RDYOUT	1
#define IOP_GETC	2
#define IOP_PUTC	3
#define IOP_SELECT	4
#define IOP_STATUS	5
#define IOP_SENSE	6
#define IOP_RESET	7

#define M7BIT		0177
#define M6BIT		0077

#define TTYDEF_HOST	"localhost"
#define TTYDEF_PORT	4200

typedef struct devdesc_t {
  char *desc;				/* text description */

  void (*dev_init)(int, char *);	/* func: initialize device */
  void (*dev_close)();			/* func: close/clear device */

  int  (*dev_rdyin)(WORD8 *);		/* func: ready for input? */
  int  (*dev_rdyout)(WORD8 *);		/* func: ready for output? */
  int  (*dev_getc)(WORD8 *);		/* func: read char */
  int  (*dev_putc)(WORD8 *);		/* func: write char */
  int  (*dev_select)(WORD8 *);		/* func: select unit */
  int  (*dev_status)(WORD8 *);		/* func: get status word */
  int  (*dev_sense)(WORD8 *);		/* func: set sense mask */
  int  (*dev_reset)(WORD8 *);		/* func: reset unit */
} DEVDESC;

static int port_base;

static int csd = -1;			/* client socket desc. */
static char in_ch = -1;			/* input char buffer */
static char out_ch = -1;		/* input char buffer */

static FILE *ppt_file;			/* file actiong as paper tape */

void *tty_write_sock(void *arg) {

  while(1) {
    if((csd > 0) && (out_ch >= 0)) {
      write(csd, &out_ch, 1);
      out_ch = -1;
    }
    usleep(3333);			/* aprox 300 cps */
  }

}

void *tty_read_sock(void *arg) {

  int rv, port;
  char *params, *host, *portstr;
  struct sockaddr_in caddr;
  struct hostent *hp;
  struct in_addr ip;

  params = (char *)arg;
  host = (char *)strtok(params, DEVDEL);
  if(host) {
    portstr = (char *)strtok(NULL, DEVDEL);
    if(portstr) {
      port = atoi(portstr);
    } else {
      port = TTYDEF_PORT;
    }
  } else {
    host = TTYDEF_HOST;
    port = TTYDEF_PORT;
  }

  hp = gethostbyname(host);
  memcpy(&ip, *(hp->h_addr_list), sizeof(struct in_addr));
  memset(&caddr, 0, sizeof(struct sockaddr_in));
  caddr.sin_family = AF_INET;
  caddr.sin_addr = ip;
  caddr.sin_port = htons(port);

  while(1) {
    if((csd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {;
      perror("ERROR: Cannot open tty socket:");
      exit(1);
    } else {
      if(connect(csd, 
		 (struct sockaddr *)&caddr, sizeof(struct sockaddr_in)) < 0) {
	perror("ERROR: TTY socket connect failed");
	exit(1);
      } else {
	while((rv = read(csd, &in_ch, 1)) > 0) {
	  /* loop */
	}
	if(rv < 0) {
	  if(errno != EINTR) {
	    perror("ERROR: Client socket read");
	    close(csd);
	    pthread_exit(NULL);
	  } else {
	    /* signal */
	  }
	} else {
	  /* server close */
	  close(csd);
	  pthread_exit(NULL);
	}
      }
    }
  }
}

void tty_init(int dev, char *devdesc) {

  pthread_t tid;
  pthread_attr_t tattr;
  int flags;
  char ebuf[255];
  struct sockaddr_in addr;

  pthread_attr_init(&tattr);
  pthread_attr_setscope(&tattr, PTHREAD_SCOPE_SYSTEM);
  if(pthread_create(&tid, &tattr, tty_read_sock, (void *)devdesc)) {
    perror("ERROR: Cannot create tty read thread:");
  }
  if(pthread_create(&tid, &tattr, tty_write_sock, NULL)) {
    perror("ERROR: Cannot create tty write thread:");
  }
}

void tty_close() {

}

int tty_reset(WORD8 *acp) {

  return 0;
}

int tty_rdyin(WORD8 *acp) {

  return (in_ch >= 0);
}

int tty_rdyout(WORD8 *acp) {

  return (out_ch < 0);
}

int tty_getc(WORD8 *acp) {
  
  char ch;

  *acp = (in_ch > 0) ? in_ch : 0;
  in_ch = -1;					   /* invalidate buffer */

  return 0;
}

int tty_putc(WORD8 *acp) {

  out_ch = (*acp & M7BIT);	/* 7 Bit ASCII */

  return 0;
}

void ppt_init(int dev, char *devdesc) {
  if((ppt_file = fopen(devdesc, "r")) == NULL ) {
    fprintf( stderr, "Error opening data file %s as PPT.\n", devdesc);
    exit(EXIT_FAILURE);
  }
}

int ppt_rdyin(WORD8 *acp) {

  return !feof(ppt_file);
}

int ppt_getc(WORD8 *acp) {

  *acp = fgetc(ppt_file) & MASK8;
  return 0;
}

static DEVDESC devices[MAXDEV] = {
  {
    "Teletype",
    tty_init,
    tty_close,
    tty_rdyin,
    tty_rdyout,
    tty_getc,
    tty_putc,
    0,
    0,
    0,
    0,
  },
  {
    "Papertape",
    ppt_init,
    0,
    ppt_rdyin,
    0,
    ppt_getc,
    0,
    0,
    0,
    0,
    0,
  },
};

void chario_init(int iobase, char *devdesc[]) {
  int i;

  port_base = iobase;
  
  for(i = 0; i < MAXDEV; i++) {
    if(devdesc[i]) {
      if(devices[i].dev_init) {
	printf("Initializing device %d, %s:", i, devices[i].desc);
	devices[i].dev_init(i, devdesc[i]);
	printf(" ok.\n");
      } else {
	fprintf(stderr, "WARNING: Unknown device: %d\n", i);
      }
    }
  }
}

void chario_close() {
  int i;
  
  for(i = 0; i < MAXDEV; i++) {
    if(devices[i].dev_close) {
      devices[i].dev_close();
    }
  }
}

int chario(int iop, int device, WORD8 *acp) {
  
  int skip = 0;
  int (*func)(WORD8 *);
  DEVDESC devdesc = devices[device];

  switch(iop) {
  case IOP_RDYIN:
    func = devdesc.dev_rdyin;
    break;
  case IOP_RDYOUT:
    func = devdesc.dev_rdyout;
    break;
  case IOP_GETC:
    func = devdesc.dev_getc;
    break;
  case IOP_PUTC:
    func = devdesc.dev_putc;
    break;
  case IOP_SELECT:
    func = devdesc.dev_select;
    break;
  case IOP_STATUS:
    func = devdesc.dev_status;
    break;
  case IOP_SENSE:
    func = devdesc.dev_sense;
    break;
  case IOP_RESET:
    func = devdesc.dev_reset;
    break;
  default:
    /* unknown device function */
    break;
  }

  if(func) {
    skip = func(acp);
  }
 
  return skip;
}
