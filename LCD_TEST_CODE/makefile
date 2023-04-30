CROSS_COMPILE=
CC 	?= $(CROSS_COMPILE)gcc
CFLAGS	?= -g -Werror -Wall
LDFLAGS ?= -pthread -lrt
TARGET = lcd
OBJFILES = wiringpi.o lcd.o
all: $(TARGET)
default: $(TARGET)
$(TARGET): $(OBJFILES)
		$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(OBJFILES)
.PHONY: clean
clean:
		rm -f *.o $(TARGET)
