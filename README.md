# IPK Project 1: IPK Calculator Protocol

Autor: Jakub Fukala

PROJ=ipkcpc
CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic
CC=gcc

$(PROJ): $(PROJ).o
$(CC) $(CFLAGS) $^ -o $(PROJ)

$(PROJ).o: $(PROJ).c $(PROJ).h
$(CC) $(CFLAGS) -c $^ -o $@


ipkcpc: ipkcpc.o
gcc ipkcpc.o -o ipkcpc

ipkcpc.o: ipkcpc.c
gcc -c ipkcpc.c -o ipkcpc.o


