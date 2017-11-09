CC=gcc
CFLAGS=-Wall -pthread

all: deadlockControl

queue.o: queue.h queue.c
	$(CC) $(CFLAGS) -c queue.c -o queue.o

deadlockControl: queue.o deadlockControl.c
	$(CC) $(CFLAGS) deadlockControl.c queue.o -o deadlockControl