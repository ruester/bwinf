/* Bundeswettbewerb Informatik
 * Aufgabe 2 - Robuttons
 *
 * Autor: Matthias Rüster
 *
 * E-Mail: ruester@molgen.mpg.de
 */

#ifndef AUFGABE2_ROBUTTON_H
#define AUFGABE2_ROBUTTON_H

#include "vektor.h"

struct robutton {
    struct vektor position,     /* Position des Robuttons */
     richtung;                  /* Richtung des Robuttons */
    char hat_muenze;            /* 1 - Robutton hat eine Muenze
                                   0 - Robutton hat keine Muenze */
};

extern struct robutton *neue_robuttons(int anzahl);

#endif
