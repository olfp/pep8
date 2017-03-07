/*
   utility functions
*/

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

static struct termios tbufsave;

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

