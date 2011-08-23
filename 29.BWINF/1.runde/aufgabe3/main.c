/* Bundeswettbewerb Informatik
 * Aufgabe 3 - Logistik
 *
 * Autor: Matthias Rüster
 *
 * E-Mail: ruester@molgen.mpg.de
 */

#include <stdio.h>
#include <stdlib.h>

#include "logistik.h"

int main(int argc, char *argv[])
{
    int **auftragsbuch;

    /* Anzahl der Parameter pruefen */
    if (argc != 2) {
        printf("Auftragsbuch als Parameter angeben\n");
        return (1);
    }

    /* Auftragsbuch erstellen */
    auftragsbuch = auftragsbuch_erstellen(argv[1]);
    
    printf("Tourenplan:\n===========\n");
    print_auftragsbuch(auftragsbuch);

    ohne_leerfahrten(auftragsbuch);
    
    mit_leerfahrten(auftragsbuch);
    
    /* Auftragsbuch wurde geaendert */
    printf("Tourenplan (inklusive Leerfahrten):\n");
    printf("===================================\n");
    print_auftragsbuch(auftragsbuch);
    
    /* Speicher freigeben */
    auftragsbuch_freigeben(auftragsbuch);

    return (0);
}
