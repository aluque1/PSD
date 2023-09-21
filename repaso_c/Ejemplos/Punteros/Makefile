CC = gcc
CFLAGS = -c -g -Wall

.SUFFIXES: .o .c
.c.o:

	$(CC)  $(CFLAGS)  $*.c

EXECS = arrays arrays2 punteros swap swap2 memDina memDina2

all: $(EXECS)

arrays2: arrays2.o
	$(CC)  arrays2.o  -o $@

arrays: arrays.o 
	$(CC)  arrays.o -o $@
	
punteros: punteros.o 
	$(CC)  punteros.o -o $@

swap: swap.o 
	$(CC)  swap.o -o $@
	
swap2: swap2.o 
	$(CC)  swap2.o -o $@
	
memDina: memDina.o 
	$(CC)  memDina.o -o $@
	
memDina2: memDina2.o 
	$(CC)  memDina2.o -o $@

clean:
	rm -f  *.o
	rm -f $(EXECS)
