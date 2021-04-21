CC=gcc
CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic -g
LDFLAGS=-pthread

all: proj2

proj2: main.o proc.o res.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f *.o proj2

zip:
	zip xzmitk01.zip *.c *.h Makefile

.PHONY: all clean zip
