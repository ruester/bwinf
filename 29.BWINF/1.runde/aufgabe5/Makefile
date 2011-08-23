CC	= gcc
CFLAGS	= -g -Wall -pedantic -O2
LDFLAGS =
OBJECTS = main.o pyramide.o transport.o koordinaten.o block.o umzug.o
PROJECT = pyramide

all: $(PROJECT)

$(PROJECT): $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) $<

clean:
	rm -f $(OBJECTS) $(PROJECT)
