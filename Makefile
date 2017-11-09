CC=gcc
CFLAGS=-W -pthread

all: deadlockControl

deadlockControl: deadlockControl.c
	$(CC) $(CFLAGS) deadlockControl.c -o deadlockControl