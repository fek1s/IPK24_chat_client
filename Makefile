EXE=ipkcpc
CFLAGS= -std=gnu99 -Wall -Wextra -Werror -pedantic
CC=gcc

ipkcpc: $(EXE).o
	@echo "Liking objects to executable"
	$(CC) $(CFLAGS) $^ -o $@

$(EXE).o: $(EXE).c
	@echo "Making object file"
	$(CC) $(CFLAGS) -c $^

clean:
	rm $(PROJ).o $(PROJ)