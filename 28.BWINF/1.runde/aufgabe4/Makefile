CC= /usr/bin/gcc
CFLAGS= -Wall -g
LDFLAGS=

all: aufgabe4

aufgabe4: aufgabe4.o
	$(CC) $(LDFLAGS) $^ -o $@ -lm

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	rm -f aufgabe4.o
	rm -f aufgabe4
