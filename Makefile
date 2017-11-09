CC=gcc
CFLAGS=-Wall -pthread

all: deadlockControl noMutex

queue.o: queue.h queue.c
	$(CC) $(CFLAGS) -c queue.c -o queue.o

deadlockControl: queue.o deadlockControl.c
	$(CC) $(CFLAGS) deadlockControl.c queue.o -o deadlockControl

noMutex: queue.o noMutex.c
	$(CC) $(CFLAGS) noMutex.c queue.o -o noMutex