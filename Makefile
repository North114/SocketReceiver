CC=gcc
LFLAGS=-lpthread
SOURCE=socket_receiver_v02.c
OBJECT=$(SOURCE:.c=.o)

all: test

test: $(OBJECT)
		$(CC) -o $@ $^ `mysql_config --libs` $(LFLAGS)

$(OBJECT): $(SOURCE)
		$(CC) -c -Wall `mysql_config --cflags` -o $@ $^

clean:
		rm *.o test
