CC=gcc
CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic -g
LDFLAGS=-pthread -lrt

all: proj2

proj2: main.o santa.o elf.o reindeer.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f *.o proj2

.PHONY: all clean
