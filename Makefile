EXE=ipkcpc
BIN= ipk24chat-client
CFLAGS= -std=gnu99 -Wall -Wextra -Werror -pedantic
CC=gcc

ipk24chat-client: $(EXE).o parse.o udp_utils.o tpc_utils.o
	@echo "Liking objects to executable"
	$(CC) $(CFLAGS) $^ -o $@

$(EXE).o: $(EXE).c tpc_utils.c parse.c udp_utils.c
	@echo "Making object file"
	$(CC) $(CFLAGS) -c $^

clean:
	rm -f *.o $(BIN)