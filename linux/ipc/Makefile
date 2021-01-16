CC      = $(CROSS_COMPILE)gcc
LD      = $(CROSS_COMPILE)ld
AR      = $(CROSS_COMPILE)ar
NM      = $(CROSS_COMPILE)nm
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
READELF = $(CROSS_COMPILE)readelf

CFLAGS += -Wall -I/include 
LDFLAGS += -lpthread

.PHONY: all ts tc shm_ss shm_cc socket_ss socket_cc
all: ts tc

ts: server.o tee_socket.o
	$(CC) -o $@ $^ $(LDFLAGS)

tc: client.o tee_socket.o
	$(CC) -o $@ $^ $(LDFLAGS)

shm_ss: shm_server.o
	$(CC) -o $@ $< $(LDFLAGS)

shm_cc: shm_client.o
	$(CC) -o $@ $< $(LDFLAGS)

socket_ss: socket_server.o
	$(CC) -o $@ $< $(LDFLAGS)

socket_cc: socket_client.o
	$(CC) -o $@ $< $(LDFLAGS)

.PHONY: clean clean-objs
clean:
	rm -f *.o
	rm -f ts tc
	rm -f shm_ss shm_cc
	rm -f socket_ss socket_cc

