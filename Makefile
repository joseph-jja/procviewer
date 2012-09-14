
CC = gcc
PKGCONFIG = `pkg-config gtk+-2.0 --cflags --libs`
CFLAGS = -Wall -g 

CFILES = procviewer.c
TARGET = procviewer

INSTALLDIR = /usr/local/bin

all:
	$(CC) $(CFLAGS) -o $(TARGET) $(CFILES) $(PKGCONFIG)

install:
	install $(TARGET) $(INSTALLDIR)

clean:
	rm -f *.o core 
