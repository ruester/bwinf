/* Bundeswettbewerb Informatik
 * Aufgabe 2 - Robuttons
 *
 * Autor: Matthias Rüster
 *
 * E-Mail: ruester@molgen.mpg.de
 */

#include <stdlib.h>
#include <stdio.h>

#include "robutton.h"

/* Beschreibung:
 *     holt Speicher fuer ein Array von Robuttonstrukturen
 * Eingabewerte:
 *     Anzahl der Robuttons
 * Rueckgabewerte:
 *     Zeiger auf das Array von Robuttons */
struct robutton *neue_robuttons(int anzahl)
{
    struct robutton *r;

    /* Speicher holen */
    if ((r = calloc(anzahl, sizeof(struct robutton))) == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    /* Zeiger zurueckgeben */
    return (r);
}
