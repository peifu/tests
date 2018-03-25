CC      = $(CROSS_COMPILE)gcc
LD      = $(CROSS_COMPILE)ld
AR      = $(CROSS_COMPILE)ar
NM      = $(CROSS_COMPILE)nm
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
READELF = $(CROSS_COMPILE)readelf

CFLAGS += -Wall -I/include 
LDFLAGS += -lpthread

.PHONY: all
all: ss cc

ss: server.o
	$(CC) -o $@ $< $(LDFLAGS) 

cc: client.o
	$(CC) -o $@ $< $(LDFLAGS) 

.PHONY: clean clean-objs
clean:
	rm -f *.o
	rm -f ss cc

