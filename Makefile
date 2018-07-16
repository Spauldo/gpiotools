CC=cc
CFLAGS=-std=c99 -Wall
EXES=gpiolist
LIBS=libgpiotools.a
OBJS=gpiotools.o gpiolist.o

.PHONY: clean all

all: $(LIBS) $(EXES)

libgpiotools.a: gpiotools.o
	ar rcs libgpiotools.a gpiotools.o

gpiotools.o: gpiotools.c gpiotools.h
	$(CC) $(CFLAGS) -c gpiotools.c

gpiolist: gpiolist.o libgpiotools.a
	$(CC) $(CFLAGS) -o gpiolist gpiolist.o -L. -lgpiotools

gpiolist.o: gpiolist.c gpiotools.h
	$(CC) $(CFLAGS) -c gpiolist.c

clean:
	rm -f $(LIBS) $(EXES) $(OBJS)
