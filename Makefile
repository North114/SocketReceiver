CC=gcc
LFLAGS=-lpthread
SOURCE=socket_receiver_v02.c

all: test

test: output.o
		$(CC) -o $@ $^ `mysql_config --libs` $(LFLAGS)

output.o: $(SOURCE)
		$(CC) -c -Wall `mysql_config --cflags` -o $@ $^

clean:
		rm *.o
