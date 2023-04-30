CROSS_COMPILE=
CC      ?= $(CROSS_COMPILE)gcc
CFLAGS  ?= -g -Werror -Wall
LDFLAGS ?= -pthread -lrt
TARGET = client
OBJFILES = wiringpi.o lcd.o client.o

all: $(TARGET)

default: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(OBJFILES)

.PHONY: clean

clean:
	rm -f *.o $(TARGET)

