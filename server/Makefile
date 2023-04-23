# Makefile for Final project
CROSS_COMPILE =
ifeq ($(CC),)
	CC = $(CROSS_COMPILE)gcc
endif
ifeq ($(CFLAGS),)
	CFLAGS = -g -Wall -Werror 
endif
ifeq ($(LDFLAGS),)
	LDFLAGS = -pthread -lrt
endif

SRC := server.c
TARGET ?= server

all: $(TARGET)

$(TARGET) : $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) server.c $(INCLUDES) $(LDFLAGS)

clean:
	-rm -f *.o $(TARGET) *.elf *.map
