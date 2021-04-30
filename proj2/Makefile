CC=gcc
CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic
LDFLAGS=-pthread
TARGETS=main.o proc.o

all: proj2

proj2: $(TARGETS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f *.o proj2

zip:
	zip proj2.zip *.c *.h Makefile

.PHONY: all clean zip
