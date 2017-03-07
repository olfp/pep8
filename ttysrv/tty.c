/*
  tty server

  (c) olf 08/05
*/

#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h> 
#include <termios.h> 
#include <signal.h> 
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "util.h"

#define BUFLEN		81

#define ESC_CHAR        035	/* Ctrl-] */
#define ESC_PROMPT	"teletype> "

#define WHTSPC		"\t "

#define	DEFAULT_PORT	4200

extern int errno;

extern int optind;
extern char *optarg;		/* argument pointer for getopt */

static int accept_sock = -1;

static void tty_quit(char *params); 
static void tty_usage(char *params); 

struct {
  char *name;
  void (*func)(char *);
} ttycmd[] = {
  {
    "q",			/* quit */
    tty_quit,
  },
  {
    "h",			/* help */
    tty_usage,
  },
  {
    "",
    NULL,
  }
};

static void usage(char *name) {
  fprintf(stderr, "%s - simulates a teletype\n", name);
  fprintf(stderr, "usage: %s [-p <port>]\n");
  fprintf(stderr, "  p - listen on given port, default %d\n", DEFAULT_PORT);
  exit(EXIT_FAILURE);
}

static void tty_quit(char *params) {
  tio_restore();
  exit(1);
}

static void tty_usage(char *params) {
  printf("quit - terminate teletype simulation.\n");
  printf("help - this text.\n");
  printf("\n");
}

static void escape(char *cmd) {

  int i;
  char *tok, *rest;

  tok = (char *)strtok_r(cmd, WHTSPC, &rest);
  for(i = 0; ttycmd[i].func; i++) {
    if(!strncmp(tok, ttycmd[i].name, strlen(ttycmd[i].name))) {
      ttycmd[i].func(rest);
    }
  }
}

static void *keyb_listen(void *arg) {

  char in_ch;
  int rv;
  static char buf[BUFLEN];

  while(1) {
    read(STDIN_FILENO, &in_ch, 1);
    if(in_ch == ESC_CHAR) {
      tio_restore();
      printf("\n%s", ESC_PROMPT);
      escape(fgets(buf, BUFLEN, stdin));
      tio_raw();
    }
    in_ch = toupper(in_ch);
    if(accept_sock > 0) {
      write(accept_sock, &in_ch, 1);
    }
  }
}


int main( int argc, char *argv[] ) {

  int listen_sock;
  int flags, rv;
  char ebuf[255];
  char optch, out_ch;
  char *prog;
  socklen_t addr_len = sizeof(struct sockaddr);
  struct sockaddr_in listen_addr, accept_addr;
  pthread_t tid;

  int port = DEFAULT_PORT;

  prog = argv[0];
  if(!(prog = strrchr(prog, '/'))) {
    prog = argv[0];
  } else {
    prog++;
  }

  while( (optch = getopt( argc, argv, "p:?" )) > 0 ) {
    switch( optch ) {
    case 'p':			/* listen on port */
      port = atoi(optarg);
      break;
    default:
      usage(prog);
      break;
    }
  }

  if( optind != argc ) {
    usage(prog);
  }

  tio_save();

  listen_sock = socket(AF_INET, SOCK_STREAM, 0);
  if(listen_sock < 0) {
    perror("ERROR: Cannot open tty socket:");
    exit(1);
  } else {
    bzero((char *) &listen_addr, sizeof(listen_addr));
    listen_addr.sin_family = AF_INET;
    listen_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    listen_addr.sin_port = htons(port);
    flags = 1;
    setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &flags, sizeof(flags));
    if(bind(listen_sock, 
	    (struct sockaddr *)&listen_addr, sizeof(listen_addr))<0) {
      sprintf(ebuf, "ERROR: Cannot bind tty socket to port %d", port);
      perror(ebuf);
      exit(1);
    } else {
      if(listen(listen_sock,1)) {
	perror("ERROR: Cannot listen to tty socket");
	exit(1);
      } else {
	tio_raw();
	if(pthread_create(&tid, NULL, keyb_listen, (void *)accept_sock)) {
	  perror("ERROR: Cannot create tty read thread:");
	}
	printf("TTY listening on port %d.\n", port);
	printf("Escape character is '^]'.\n");
	while(1) {
	  accept_sock = accept(listen_sock, 
			       (struct sockaddr *) &accept_addr, &addr_len);
	  if(accept_sock < 0) {
	    perror("ERROR: Client socket accept failed");
	    exit(1);
	  } else {
	    while((rv = read(accept_sock, &out_ch, 1)) > 0) {
	      putchar(toupper(out_ch));
	      fflush(stdout);
	    }
	    if((rv < 0) && (errno != EINTR)) {
	      perror("ERROR: Client socket read");
	    } else {
	      /* client close */
	    }
	  }
	}
      }
    }
  }
  tio_restore();
}
