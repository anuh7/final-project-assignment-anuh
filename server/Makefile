# Makefile for Final project
CROSS_COMPILE=
CC 	?= $(CROSS_COMPILE)gcc
CFLAGS	?= -g -Werror -Wall
LDFLAGS ?= -pthread -lrt
SRC = client.c
OUTPUT = client

all:
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(OUTPUT) $(SRC) 

clean:
	-rm -f *.o $(OUTPUT) *.elf *.map
