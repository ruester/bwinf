# 29. Bundeswettbewerb Informatik - 2. Runde
# Aufgabe 1 - Kisten in Kisten in Kisten
#
# Autor: Matthias Rüster
#
# E-Mail: ruester@molgen.mpg.de

CC	= gcc
CFLAGS	= -g -Wall -pedantic
LDFLAGS =
OBJECTS = main.o kiste.o keller.o
PROJECT = kisteninkisten

all: $(PROJECT)

$(PROJECT): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) $^

clean:
	rm -f $(OBJECTS) $(PROJECT)
