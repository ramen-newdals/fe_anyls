# makefile for truss simulation

C=gcc
CFLAGS=-O2 -g -Wall -Wfatal-errors -Wconversion
LDFLAGS=-O2 -g -lm -Wall -Wfatal-errors -Wconversion -Llog.h
all: truss

truss: logging.o truss_element.o
	$(C) $(LDFLAGS) -o $@ $^

logging.o: logging.c logging.h
	$(C) -c $(CFLAGS) -o logging.o logging.c

truss_element.o: truss_element.c logging.h
	$(C) -c $(CFLAGS) -o truss_element.o truss_element.c

run: truss
	./truss

clean:
	$(RM) truss_element.o logging.o

.PHONY: all clean run