CC      = gcc
CFLAGS  = -Wall -g -Wextra -Wunused -O3
LDFLAGS = -lm -lgd -lpthread -O3

JAVAC  = javac
JFLAGS = -Xlint:all -Werror

PROJECT = kool
OBJECTS = main.o cube.o labyrinth.o rectangle.o

CLASSES = Main.class GUI.class Cube.class

all: $(PROJECT) $(CLASSES)

$(PROJECT): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

%.class: %.java
	$(JAVAC) $(JFLAGS) $^

clean:
	rm -f $(OBJECTS) $(PROJECT) $(CLASSES) *.class
