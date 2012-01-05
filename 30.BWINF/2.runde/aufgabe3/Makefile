CC      = gcc
CFLAGS  = -Wall -g -Wextra -Wunused -O3
LDFLAGS = -lgmp

PROJECT = trickey
OBJECTS = main.o saison.o team.o

all: $(PROJECT)

$(PROJECT): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm -f $(OBJECTS) $(PROJECT)
