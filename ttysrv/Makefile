#
#  Makefile for tty server
#


CC = gcc

TTY = ../bin/teletype

BINDIR = /usr/local/bin

CFLAGS =  -pedantic -g $(GNURL)
LDFLAGS = -g

#LIBS = -lpthread -lsocket
LIBS = -lpthread 

OBJS = tty.o util.o

all: $(TTY)

$(TTY): $(OBJS)
	$(CC) $(LDFLAGS) -o $(TTY) $(OBJS) $(LIBS)

clean:
	rm -f $(OBJS) $(TTY)

install:
	install -m 755 $(TTY) $(BINDIR)
