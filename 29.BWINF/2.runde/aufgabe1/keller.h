/* 29. Bundeswettbewerb Informatik - 2. Runde
 * Aufgabe 1 - Kisten in Kisten in Kisten
 *
 * Autor: Matthias Rüster
 *
 * E-Mail: ruester@molgen.mpg.de
 */

#ifndef AUFGABE1_KELLER_H
#define AUFGABE1_KELLER_H

#include "kiste.h"

struct keller {
    struct kiste **kiste;       /* Array von Kisten */
    int anzahl_kisten;          /* Groesse des Arrays */
    unsigned long aeusseres_volumen, /* Volumen der nicht verpackten Kisten */
                  gesamt_volumen;    /* Volumen aller Kisten */
};

extern struct keller *neuer_keller_von_datei(char *dateiname);

extern void keller_loeschen(struct keller *k);

extern void print_kisten(struct keller *k);

extern char finde_loesung_schnell(struct keller *k);

extern void finde_loesung_quickbruteforce(struct keller *k);

extern void finde_loesung_minimaler_zwischenraum(struct keller *k);

extern void print_anleitung(struct keller *k);

extern void keller_kopieren(struct keller *von, struct keller *nach);

extern struct keller *kopie_von_keller(struct keller *k);

#endif
