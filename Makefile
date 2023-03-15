# makefile for truss simulation

C=gcc
CFLAGS=-O2 -g -Wall -Wfatal-errors -Wconversion
LDFLAGS=-O2 -g -lm -Wall -Wfatal-errors -Wconversion -Llog.h

all: truss

# truss is for building matrix generation code
truss: logging.o truss_element.o
	$(C) $(LDFLAGS) -o $@ $^

logging.o: logging.c logging.h
	$(C) -c $(CFLAGS) -o logging.o logging.c

truss_element.o: truss_element.c logging.h
	$(C) -c $(CFLAGS) -o truss_element.o truss_element.c

# read_matrix is for reading in matricies from matrix market
read_matrix: read_matrix.o mmio.o
	$(C) $(LDFLAGS) -o $@ $^

read_matrix.o: read_matrix.c
	$(C) -c $(CFLAGS) -o read_matrix.o read_matrix.c

mmio.o: mmio.c mmio.h
	$(C) -c $(CFLAGS) -o mmio.o mmio.c

run: truss
	./truss

clean:
	$(RM) truss_element.o logging.o

.PHONY: all clean run