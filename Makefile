EXE=ipk24chat-client
CFLAGS= -std=gnu99 -Wall -Wextra -Werror -pedantic
CC=gcc

ipkcpc: $(EXE).o ipkcpc_utils.o
	@echo "Liking objects to executable"
	$(CC) $(CFLAGS) $^ -o $@

$(EXE).o: $(EXE).c ipkcpc_utils.c ipkcpc_utils.h
	@echo "Making object file"
	$(CC) $(CFLAGS) -c $^

clean:
	rm $(PROJ).o $(PROJ)