/* Bundeswettbewerb Informatik
 * Aufgabe 5 - Pyramide
 *
 * Autor: Matthias Rüster
 *
 * E-Mail: ruester@molgen.mpg.de
 */

#ifndef AUFGABE5_UMZUG_H
#define AUFGABE5_UMZUG_H

#include "pyramide.h"
#include "transport.h"

struct pyramidenumzug {
    struct pyramide *theben, *luxor;
    struct transportliste *umzug;    /* Anleitung fuer den Umzug */
};

extern struct pyramidenumzug *neuer_pyramidenumzug(int n);
extern void umzug(struct pyramidenumzug *p);
extern void print_anleitung(struct pyramidenumzug *p);
extern void pyramidenumzug_freigeben(struct pyramidenumzug *p);

#endif
