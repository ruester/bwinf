CC      = gcc
CFLAGS  = -Wall -g -Wextra -Wunused -O2
LDFLAGS = -O2 -lm

PROJECT = aufgabe2
OBJECTS = main.o

all: $(PROJECT)

$(PROJECT): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm -f $(OBJECTS) $(PROJECT)
