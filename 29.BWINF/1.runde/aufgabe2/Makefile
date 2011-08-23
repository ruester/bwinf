SDL_CFLAGS  = `pkg-config --cflags sdl`
SDL_LDFLAGS = `pkg-config --libs sdl`
CC	= gcc
CFLAGS	= -g -Wall -pedantic
LDFLAGS = -lm
OBJECTS = main.o simulation.o robutton.o vektor.o tisch.o kollision.o
PROJECT = robuttons

all: $(PROJECT)

$(PROJECT): $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS) $(SDL_LDFLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) $(SDL_CFLAGS) $<

clean:
	rm -f $(OBJECTS) $(PROJECT)
