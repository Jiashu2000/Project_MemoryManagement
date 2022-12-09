# Makefile for mem memory system, CSE374 22WI
# Name: Jiashu Chen
# Date: 07/12/2022

CC = gcc
CARGS = -Wall -std=c11 -DNDEBUG

all: bench

# basic build
bench: bench.o getmem.o freemem.o mem_utils.o
	$(CC) $(CARGS) -o bench $^

# object files
# fill in your individual object targets here

bench.o: bench.c mem.h
	gcc -Wall -std=c11 -g -c bench.c

getmem.o: getmem.c mem.h mem_impl.h
	gcc -Wall -std=c11 -g -c getmem.c

freemem.o: freemem.c mem.h mem_impl.h
	gcc -Wall -std=c11 -g -c freemem.c

mem_utils.o: mem_utils.c mem.h mem_impl.h
	gcc -Wall -std=c11 -g -c mem_utils.c



# You can use these examples for other types of builds
# make sure you know what they do.
noassert: CARGS += -D NDEBUG
noassert: bench

debug: CARGS += -g
debug: bench

test: debug
	./bench 10 50

clean:
	rm -f bench *.o *~ 
