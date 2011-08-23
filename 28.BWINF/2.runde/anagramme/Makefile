CC= /usr/bin/gcc
CFLAGS= -Wall -g -pedantic -Wextra
LDFLAGS=

all: anagramme

anagramme: anagramme.o
	$(CC) $(LDFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	rm -f anagramme.o
	rm -f anagramme
