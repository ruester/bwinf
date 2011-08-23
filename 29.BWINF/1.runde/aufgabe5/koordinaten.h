/* Bundeswettbewerb Informatik
 * Aufgabe 5 - Pyramide
 *
 * Autor: Matthias Rüster
 *
 * E-Mail: ruester@molgen.mpg.de
 */

#ifndef AUFGABE5_KOORDINATEN_H
#define AUFGABE5_KOORDINATEN_H

struct koordinate {
    int k, x, y;
    struct koordinate *next;
};

struct koordinatenliste {
    struct koordinate *first, *last;
};

extern struct koordinatenliste *neue_koordinatenliste();
extern struct koordinate *neue_koordinate(int k, int x, int y);
extern void loesche_koordinate(struct koordinatenliste *kl,
                               struct koordinate *k);
extern void loesche_koordinatenliste(struct koordinatenliste *l);
extern void koordinate_hinzufuegen(struct koordinatenliste *l,
                                   struct koordinate *k);
extern void print_koordinatenliste(struct koordinatenliste *l);

#endif
