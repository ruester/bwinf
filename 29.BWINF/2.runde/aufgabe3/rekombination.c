/* 29. Bundeswettbewerb Informatik - 2. Runde
 * Aufgabe 3 - Traumdreiecke
 *
 * Autor: Matthias Rüster
 *
 * E-Mail: ruester@molgen.mpg.de
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "util.h"

/* Beschreibung:
 *     - rekombiniert zwei Traumdreiecke
 *     - speichert das Ergebnis im dritten Argument
 * Eingabewerte:
 *     1. und 2. Argument: Zeiger auf die Traumdreieck-Eltern
 *     3. Argument: Zeiger auf das Traumdreieck-Kind
 * Rueckgabewerte:
 *     keine */
static struct traumdreieck *rekombinieren(struct traumdreieck *vater,
                                          struct traumdreieck *mutter)
{
    int i, j, split;
    struct traumdreieck *kind;

    /* zufaellige Ebene bestimmen */
    split = rand() % vater->anzahl_ebenen;

    kind = neues_traumdreieck(vater->anzahl_ebenen);

    /* dem Kind die Farbwerte der Eltern geben */
    for (i = 0; i < vater->anzahl_ebenen; i++)
        for (j = 0; j <= i; j++)
            if (i >= split)
                /* Farbwerte des Vaters uebernehmen */
                kind->murmel[i][j] = vater->murmel[i][j];
            else
                /* Farbwerte der Mutter uebernehmen */
                kind->murmel[i][j] = mutter->murmel[i][j];
    
    return kind;
}

int main(int argc, char *argv[])
{
    struct traumdreieck *vater, *mutter, *kind;
    
    /* Parameter ueberpruefen */
    if (argc != 2) {
        fprintf(stderr, "Falsche Parameter\n");
        exit(EXIT_FAILURE);
    }
    
    srand(time(NULL) + getpid());
    
    /* die beiden besten Traumdreiecke laden */
    vater = traumdreieck_laden("1.sel");
    mutter = traumdreieck_laden("2.sel");
    
    /* Traumdreiecke rekombinieren */
    kind = rekombinieren(vater, mutter);
    
    traumdreieck_loeschen(vater);
    traumdreieck_loeschen(mutter);
    
    traumdreieck_speichern(kind, argv[1]);
    
    traumdreieck_loeschen(kind);
    
    return 0;
}
