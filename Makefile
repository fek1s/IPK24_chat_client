EXE=ipkcpc
CFLAGS= -std=gnu99 -Wall -Wextra -Werror -pedantic
CC=gcc

ipk24chat-client: $(EXE).o ipkcpc_utils.o parse.o
	@echo "Liking objects to executable"
	$(CC) $(CFLAGS) $^ -o $@

$(EXE).o: $(EXE).c tpc_utils.c parse.c udp_utils.c
	@echo "Making object file"
	$(CC) $(CFLAGS) -c $^

clean:
	rm ipkcpc_utils.o parse.o ipkcpc.o ipk24chat-client