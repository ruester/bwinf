/* Bundeswettbewerb Informatik
 * Aufgabe 5 - Pyramide
 *
 * Autor: Matthias Rüster
 *
 * E-Mail: ruester@molgen.mpg.de
 */

#ifndef AUFGABE5_TRANSPORT_H
#define AUFGABE5_TRANSPORT_H

#include "pyramide.h"

#define LAGER (-1)

struct transport {
    int k, x, y,                /* Ursprungs-Koordinaten */
     k_ziel, x_ziel, y_ziel;    /* Ziel-Koordinaten */
    char blocktyp;
    struct transport *next;
};

struct transportliste {
    int anzahl_transporte;
    struct transport *first, *last;
};

extern struct transportliste *neue_transportliste();

extern struct transport *neuer_transport(int k, int x, int y,
                                         int k_ziel, int x_ziel, int y_ziel,
                                         char typ);

extern void transport_hinzufuegen(struct transportliste *l,
                                  struct transport *t);

extern void print_transport(struct transport *t);

extern void print_transportliste(struct transportliste *l);

extern void transportliste_freigeben(struct transportliste *l);

#endif
