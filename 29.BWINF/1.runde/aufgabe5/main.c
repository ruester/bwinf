/* Bundeswettbewerb Informatik
 * Aufgabe 5 - Pyramide
 *
 * Autor: Matthias Rüster
 *
 * E-Mail: ruester@molgen.mpg.de
 */

#include <stdio.h>
#include <stdlib.h>

#include "umzug.h"

int main(int argc, char *argv[])
{
    struct pyramidenumzug *p;
    int n;

    /* Anzahl der Parameter ueberpruefen */
    if (argc != 2) {
        printf("Anzahl der Schichten als Parameter angeben\n");
        return (1);
    }

    /* Anzahl der Schichten einlesen */
    n = atoi(argv[1]);

    /* Parameter ueberpruefen */
    if (n <= 0) {
        printf("N muss groesser als 0 sein\n");
        return (1);
    }

    /* neuen Pyramidenumzug erstellen */
    p = neuer_pyramidenumzug(n);

    /* die Pyramide von Theben nach Luxor transportieren */
    umzug(p);

    /* pruefen ob Pyramide wirklich gebaut wurde */
    if (!ist_fertig(p->luxor))
        fprintf(stderr, "Pyramide konnte nicht gebaut werden\n");

    /* Anleitung ausgeben */
    print_transportliste(p->umzug);

    /* Speicher freigeben */
    pyramidenumzug_freigeben(p);

    return (0);
}
