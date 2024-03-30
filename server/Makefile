EXE=ipk_server
CFLAGS= -std=gnu99 -Wall -Wextra -Werror -pedantic
CC=gcc

ipk_server: $(EXE).o
	@echo "Liking objects to executable"
	$(CC) $(CFLAGS) $^ -o $@

$(EXE).o: $(EXE).c
	@echo "Making object file"
	$(CC) $(CFLAGS) -c $^

clean:
	@echo "Cleaning up"
	rm -f *.o $(EXE)