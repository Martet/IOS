CC=gcc
CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic -g

all: proj2

proj2: main.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f *.o proj2

.PHONY: all clean
