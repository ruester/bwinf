/* 29. Bundeswettbewerb Informatik - 2. Runde
 * Aufgabe 3 - Traumdreiecke
 *
 * Autor: Matthias Rüster
 *
 * E-Mail: ruester@molgen.mpg.de
 */

#ifndef AUFGABE3_PARALLEL_H
#define AUFGABE3_PARALLEL_H

struct traumdreieck {
    unsigned char **murmel;
    int anzahl_ebenen, anzahl_farben;
};

extern struct traumdreieck *neues_traumdreieck(int n);

extern void traumdreieck_loeschen(struct traumdreieck *t);

extern void traumdreieck_speichern(struct traumdreieck *t, char *dateiname);

extern void loesung_speichern(struct traumdreieck *t, char *dateiname);

extern struct traumdreieck *traumdreieck_laden(char *dateiname);

#endif
