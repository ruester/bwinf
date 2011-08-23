CC= /usr/bin/gcc
CFLAGS= -Wall -g
LDFLAGS=

all: aufgabe2

aufgabe2: aufgabe2.o
	$(CC) $(LDFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	rm -f aufgabe2.o
	rm -f aufgabe2
