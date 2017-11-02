CC      = $(CROSS_COMPILE)gcc
LD      = $(CROSS_COMPILE)ld
AR      = $(CROSS_COMPILE)ar
NM      = $(CROSS_COMPILE)nm
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
READELF = $(CROSS_COMPILE)readelf

OBJS = update_key_test.o

CFLAGS += -Wall -I/include 
LDFLAGS += -lcrypto -lssl

.PHONY: all
all: update_key_test

update_key_test: $(OBJS)
	$(CC) -o $@ $< $(LDFLAGS) 

.PHONY: clean clean-objs
clean:
	rm -f $(OBJS)
	rm -f update_key_test

