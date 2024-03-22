EXE=ipkcpc
CFLAGS= -std=gnu99 -Wall -Wextra -Werror -pedantic
CC=gcc

ipk24chat-client: $(EXE).o ipkcpc_utils.o
	@echo "Liking objects to executable"
	$(CC) $(CFLAGS) $^ -o $@

$(EXE).o: $(EXE).c ipkcpc_utils.c
	@echo "Making object file"
	$(CC) $(CFLAGS) -c $^

clean:
	rm $(PROJ).o $(PROJ)