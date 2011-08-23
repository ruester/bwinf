CC= /usr/bin/gcc
CFLAGS= -Wall -g
LDFLAGS=

all: aufgabe5

aufgabe5: aufgabe5.o
	$(CC) $(LDFLAGS) $^ -o $@ -lm

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	rm -f aufgabe5.o
	rm -f aufgabe5
