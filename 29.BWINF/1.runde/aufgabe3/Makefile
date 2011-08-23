CC	= gcc
CFLAGS	= -g -Wall -pedantic
LDFLAGS =
OBJECTS = main.o logistik.o
PROJECT = logistisch

all: $(PROJECT)

$(PROJECT): $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) $<

clean:
	rm -f $(OBJECTS) $(PROJECT)
