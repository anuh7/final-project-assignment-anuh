ifeq ($(CC),)
	CC= $(CROSS_COMPILE)gcc
endif

ifeq ($(CFLAGS),)
	CFLAGS= -g -Wall
endif
 
ifeq ($(LDFLAGS),)
	LDFLAGS= -pthread 
endif


TARGET1= bme280 

SRCS1:= bme280.c \
       compensation.c \
       compensation.h 


default: $(TARGET1)
all: $(TARGET1) 


$(TARGET1): $(SRCS1) 
	$(CC) $(CFLAGS) $^ -o $@ $(INCLUDES) $(LDFLAGS)

clean: 	
	rm -f *.o $(TARGET1) *.elf *.map

