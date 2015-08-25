CC=gcc
LFLAGS=-lpthread
CFLAGS=-c -Wall
SOURCE=thread_1.c

all:output

output:output.o
	$(CC) $(LFLAGS) -o $@ $^

output.o:$(SOURCE)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm *.o
