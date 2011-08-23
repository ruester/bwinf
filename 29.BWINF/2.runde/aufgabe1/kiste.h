/* 29. Bundeswettbewerb Informatik - 2. Runde
 * Aufgabe 1 - Kisten in Kisten in Kisten
 *
 * Autor: Matthias Rüster
 *
 * E-Mail: ruester@molgen.mpg.de
 */

#ifndef AUFGABE1_KISTE_H
#define AUFGABE1_KISTE_H

struct kiste {
    int l, b, h,             /* Laenge, Breite und Hoehe der Kiste */
     index;                  /* zum Kopieren der Kellerstrukturen benoetigt */
    char verpackt;           /* 1 - Kiste ist verpackt
                                0 - Kiste ist nicht verpackt */
    unsigned long volumen;   /* aeusseres Volumen der Kiste (l * b * h) */
    struct kiste *inhalt[2]; /* Zeiger auf die enthaltenen Kisten
                                inhalt[0] == NULL: Kiste ist leer
                                inhalt[1] == NULL: eine Kiste ist verpackt
                                inhalt[1] != NULL: zwei Kisten sind verpackt */
};

extern struct kiste *neue_kiste(int l, int b, int h, int index);

extern void kiste_kopieren(struct kiste *von, struct kiste *nach);

extern char kann_gepackt_werden(struct kiste *von, struct kiste *nach);

extern void kiste_packen(struct kiste *von, struct kiste *nach);

/* Kiste a aus Kiste b herausnehmen */
extern void kiste_auspacken(struct kiste *a, struct kiste *b);

extern char koennen_gepackt_werden(struct kiste *von, struct kiste *von2,
                                   struct kiste *nach);

extern void kisten_packen(struct kiste *von, struct kiste *von2,
                          struct kiste *nach);

/* Kiste a und b aus Kiste c herausnehmen */
extern void kisten_auspacken(struct kiste *a, struct kiste *b,
                             struct kiste *c);

#endif
