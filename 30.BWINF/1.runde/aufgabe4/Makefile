CC      = gcc
CFLAGS  = -Wall -g -Wextra -Wunused -O2
LDFLAGS = -O2

PROJECT = aufgabe4
OBJECTS = main.o hash.o entry.o tree.o

all: $(PROJECT)

$(PROJECT): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm -f $(OBJECTS) $(PROJECT)
