/*
  pep8 simulator

  character i/o 
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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
#include "types8.h"

#ifdef __raspi__
#include "raspi/tm1637.h"
#endif

#define IOP_RDYIN	0
#define IOP_RDYOUT	1
#define IOP_GETC	2
#define IOP_PUTC	3
#define IOP_SELECT	4
#define IOP_STATUS	5
#define IOP_SENSE	6
#define IOP_RESET	7

#define TTYDEF_HOST	"localhost"
#define TTYDEF_PORT	4200

#define SCODEF_HOST	"localhost"
#define SCODEF_PORT	4321

#define SEGDEF_CLKPIN	12

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

static int  csd = -1;			/* tty client socket desc. */
static unsigned char in_ch = BIT7;	/* tty input char buffer */
static unsigned char out_ch = BIT7;	/* tty input char buffer */
static WORD8 tty_mask = 0;		/* tty sense mask for interrupts */

static FILE *ppt_file;			/* file acting as paper tape */

static int ssd = -1;			/* scope client socket desc. */
static int out_sx = -1;			/* scope out x buffer */
static int out_sy = -1;			/* scope out y buffer */
static int out_si = -1;			/* scope intensify */
static int buf_sx = -1;
static int buf_sy = -1;
static int rdydly = 0;			/* ready delay in microsecs */

void *tty_write_sock(void *arg) {

  while(1) {
    if((csd > 0) && !(out_ch & BIT7)) {
      write(csd, &out_ch, 1);
      out_ch = BIT7;
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
      if(connect(csd, (struct sockaddr *)&caddr, sizeof(struct sockaddr_in)) < 0) {
	perror("ERROR: TTY socket connect failed");
	exit(1);
      } else {
	while((rv = read(csd, &in_ch, 1)) > 0) {
	  /* poll loop */
	  if(tty_mask & TTYKIE) {	/* keyboard interrupt enabled */
	    irq8 = 1;			/* request interrupt */
	  }
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

int tty_rdyin(WORD8 *acp) {

  return ((in_ch & BIT7) == 0);
}

int tty_rdyout(WORD8 *acp) {

  return ((out_ch & BIT7) > 0);
}

int tty_getc(WORD8 *acp) {
  
  unsigned char ch;

  *acp = ((in_ch & BIT7) == 0) ? in_ch : 0;
  in_ch = BIT7;					   /* invalidate buffer */

  return 0;
}

int tty_putc(WORD8 *acp) {

  out_ch = (*acp & MASK7);	/* 7 Bit ASCII */
  return 0;
}

int tty_smask(WORD8 *acp) {

  tty_mask = *acp;

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

void *sco_write_sock(void *arg) {
  int rv, port;
  char *params, *host, *portstr;
  struct sockaddr_in caddr;
  struct hostent *hp;
  struct in_addr ip;
  char sbuf[24];
  int posx, posy;


  params = (char *)arg;
  host = (char *)strtok(params, DEVDEL);
  if(host) {
    portstr = (char *)strtok(NULL, DEVDEL);
    if(portstr) {
      port = atoi(portstr);
    } else {
      port = SCODEF_PORT;
    }
  } else {
    host = SCODEF_HOST;
    port = SCODEF_PORT;
  }

  hp = gethostbyname(host);
  memcpy(&ip, *(hp->h_addr_list), sizeof(struct in_addr));
  memset(&caddr, 0, sizeof(struct sockaddr_in));
  caddr.sin_family = AF_INET;
  caddr.sin_addr = ip;
  caddr.sin_port = htons(port);

  while(1) {
    if((ssd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {;
      perror("ERROR: Cannot open scope socket:");
      exit(1);
    } else {
      if(connect(ssd, (struct sockaddr *)&caddr, sizeof(struct sockaddr_in)) < 0) {
	perror("ERROR: Scope socket connect failed");
	exit(1);
      } else {
	while(1) {
	  if((ssd > 0) && (out_si > 0)) {
	    if(out_sx & BIT9) {
	      posx = SGNOFF - ((~out_sx & MASK8) + 1);
	    } else {
	      posx = SGNOFF + out_sx;
	    }
	    if(out_sy & BIT9) {
	      posy = SGNOFF + ((~out_sy & MASK8) + 1);
	    } else {
	      posy = SGNOFF - out_sy;
	    }

	    sprintf(sbuf, "%d,%d\n", posx, posy);
	    if((rv = write(ssd, sbuf, strlen(sbuf))) < 0) {
	      if(errno != EINTR) {
		perror("ERROR: Scope client socket write");
		close(ssd);
		pthread_exit(NULL);
	      } else {
		/* signal */
	      }
	    }
	    out_si = -1;
	  }
	  usleep(1);
	  if(rdydly)
	    rdydly--;
	}
      }
    }
  }
}

void sco_init(int dev, char *devdesc) {

  pthread_t tid;
  pthread_attr_t tattr;
  int flags;
  char ebuf[255];
  struct sockaddr_in addr;

  pthread_attr_init(&tattr);
  pthread_attr_setscope(&tattr, PTHREAD_SCOPE_SYSTEM);
  if(pthread_create(&tid, &tattr, sco_write_sock, (void *)devdesc)) {
    perror("ERROR: Cannot create tty write thread:");
  }
}

int sco_rdyout(WORD8 *acp) {

  if(rdydly <= 0) {
    out_sx = buf_sx;
    out_sy = buf_sy;
  }

  return (rdydly <= 0);
}

int sco_putx(WORD8 *acp) {

  rdydly = 2;
  buf_sx = (*acp & MASK9);	/*  512 pix */

  return 0;
}

int sco_puty(WORD8 *acp) {

  rdydly = 2;
  buf_sy = (*acp & MASK9);	

  return 0;
}

int sco_puti(WORD8 *acp) {

  out_si = 1; /* *acp; */

  return 0;
}

#ifdef __raspi__

void checkuid(int dev) {
  if(geteuid()) {
    fprintf(stderr, "Must be root for GPIO access!\n");
    exit(dev);
  }
}

void seg_init(int dev, char *devdesc) {
  /* must be root to access GPIO */
  checkuid(dev);

  int pin = atoi(devdesc);
  if(pin == 0) {
    pin = SEGDEF_CLKPIN;
  }
  tm1637_init(pin, pin+1, false, 1.0);
  tm1637_clear();
}

void seg_close() {
  tm1637_cleanup();
}

static int seg_digit = 0;

int seg_putc(WORD8 *acp) {
printf("7SEG: %d at pos %d\n", *acp, seg_digit);
  tm1637_show_1(seg_digit, (*acp & MASK7));

  return 0;
}

int seg_seld(WORD8 *acp) {
  seg_digit = (*acp & MASK7);

  return 0;
}

int seg_reset(WORD8 *acp) {
  tm1637_clear();

  return 0;
}

static int pio_offset = 2;
static int pio_usebits = 0;

void pio_init(int dev, char *devdesc) {
  /* must be root to access GPIO */
  checkuid(dev);
  if (gpioInitialise()<0) exit(42);
}

int pio_read(WORD8 *acp) {
  uint32_t bits = gpioRead_Bits_0_31();
  /* rintf("%08x - * %08x *\n", bits, (bits >> 6) & 0xf); */

  bits = bits >> pio_offset;
  
  *acp = (bits & MASK12);

  return 0;
}

int pio_write(WORD8 *acp) {
  int bits = *acp;

  for (int p = 0; p < 11; p++) {
    if(pio_usebits & (1 << p)) {
      /* printf("PIO write %d <- 0x%02x\n", p + pio_offset, (bits & (1 << p)) ? 1 : 0); */
      gpioWrite(p + pio_offset, (bits & (1 << p)) ? 1 : 0);
    }
  }

  return 0;
}

int pio_dirs(WORD8 *acp) {
  int mask = *acp;

  for (int p = 0; p < 11; p++) {
    if(pio_usebits & (1 << p)) {
      int dir = mask & (1 << p) ? PI_OUTPUT : PI_INPUT;
      /* printf("PIO dir %d <- 0x%02x\n", p + pio_offset, dir); */
      gpioSetMode(p + pio_offset, dir);
      if(dir == PI_INPUT) {
        /* printf("PIO pulldown %d <- 0x%02x\n", p + pio_offset, PI_PUD_DOWN); */
	gpioSetPullUpDown(p + pio_offset, PI_PUD_DOWN);
      } 
    }
  }

  return 0;
}

int pio_mask(WORD8 *acp) {
  pio_usebits = *acp;

  return 0;
}

#endif

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
    tty_smask,
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
  {
    "Scope",
    sco_init,
    0,
    0,
    sco_rdyout,
    sco_putx,
    sco_puty,
    sco_puti,
    0,
    0,
    0,
  },
#ifdef __raspi__
  {
    "7Segment",
    seg_init,
    seg_close,
    0,
    0,
    0,
    seg_putc,
    seg_seld,
    0,
    0,
    seg_reset,
  },
  {
    "GPIO",
    pio_init,
    0,
    0,
    0,
    pio_read,
    pio_write,
    pio_dirs,
    0,
    pio_mask,
    0,
  }
#endif
};

void chario_show() {
  int i;

  for(i = 0; i < MAXDEV; i++) {
    if(devices[i].desc) {
      printf("%d: %s\n", i, devices[i].desc);
    }
  }
}

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
