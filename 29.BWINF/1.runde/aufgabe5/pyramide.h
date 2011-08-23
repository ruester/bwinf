/* Bundeswettbewerb Informatik
 * Aufgabe 5 - Pyramide
 *
 * Autor: Matthias Rüster
 *
 * E-Mail: ruester@molgen.mpg.de
 */

#ifndef AUFGABE5_PYRAMIDE_H
#define AUFGABE5_PYRAMIDE_H

#include "koordinaten.h"

struct pyramide {
    char ***block;
    int schichten;
    struct koordinatenliste *kl;  /* Liste von Koordinaten */
};

extern void baue_pyramide(struct pyramide *p);
extern struct pyramide *neue_pyramide(int n);
extern void pyramide_freigeben(struct pyramide *p);
extern void print_pyramide(struct pyramide *p);
extern void update_liste(struct pyramide *p);
extern char ist_fertig(struct pyramide *p);
extern char ist_gestuetzt(struct pyramide *p, int k, int x, int y);
extern struct koordinate *hole_block(struct pyramide *p, char typ);
extern int ist_abholbar(struct pyramide *p, int typ);

#endif
