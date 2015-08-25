#!/bin/bash
#a simple script for lazy Programmer
FILENAME=socket_receiver_v02

gcc -c `mysql_config --cflags` $FILENAME.c

gcc -o output $FILENAME.o `mysql_config --libs` -lpthread
