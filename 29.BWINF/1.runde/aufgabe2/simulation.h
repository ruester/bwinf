/* Bundeswettbewerb Informatik
 * Aufgabe 2 - Robuttons
 *
 * Autor: Matthias Rüster
 *
 * E-Mail: ruester@molgen.mpg.de
 */

#ifndef AUFGABE2_SIMULATION_H
#define AUFGABE2_SIMULATION_H

#include <SDL.h>

/* Konstanten */
#define MAX_FRAMESKIP 1000
#define MAX_ZOOMLEVEL 16
#define F_BREITE 800
#define F_HOEHE  800

struct simulation {
    struct robutton *robuttons;
    struct vektor *neue_positionen;
    struct vektor *muenzen;
    struct tisch *tisch;
    SDL_Surface *screen;                   /* Ausgabefenster */
    char tischflaeche[F_HOEHE][F_BREITE];  /* macht die Ausgabe schneller */
    int anzahl_muenzen, anzahl_robuttons,
     von_x, von_y, bis_x, bis_y,   /* zum Zooomen */
     frameskip;                    /* zur Beschleunigung der Simulation */
};

extern void simulation_starten(char *konfiguration);

#endif
