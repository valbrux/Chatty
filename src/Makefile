#
#
# Chatty Makefile
#
#
CC = gcc
LD = gcc
BIN = ../bin

chatty : chatty.o chatty_lib.o
	$(LD) -o $(BIN)/chatty chatty.o chatty_lib.o

chatty.o : chatty.c ../include/chatty.h
	$(CC) -c chatty.c

chatty_lib.o : chatty_lib.c ../include/chatty.h
	$(CC) -c chatty_lib.c

