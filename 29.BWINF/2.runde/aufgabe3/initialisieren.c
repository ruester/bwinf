/* 29. Bundeswettbewerb Informatik - 2. Runde
 * Aufgabe 3 - Traumdreiecke
 *
 * Autor: Matthias Rüster
 *
 * E-Mail: ruester@molgen.mpg.de
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#include "util.h"

/* Beschreibung:
 *     setzt die Murmeln des Traumdreiecks mit zufaelligen Farbwerten
 * Eingabewert:
 *     Zeiger auf das Traumdreieck
 * Rueckgabewerte:
 *     keine */
static void random_traumdreieck(struct traumdreieck *t)
{
    int i, j;

    for (i = 0; i < t->anzahl_ebenen; i++)
        for (j = 0; j <= i; j++)
            t->murmel[i][j] = 1 + rand() % t->anzahl_farben;
}

int main(int argc, char *argv[])
{
    char dateiname[NAME_MAX + 1];
    int i, population, n;
    struct traumdreieck *t;

    /* Parameter einlesen und pruefen */
    if (argc != 3
        || (n = atoi(argv[1])) < 1 || n > 1000
        || (population = atoi(argv[2])) < 2) {
        fprintf(stderr, "Falsche Parameter\n");
        exit(EXIT_FAILURE);
    }
    
    srand(time(NULL) + getpid());
    
    t = neues_traumdreieck(n);
    
    for (i = 1; i <= population; i++) {
        /* Dateiname erstellen */
        if (sprintf(dateiname, "%d.td", i) < 0) {
            perror("sprintf");
            exit(EXIT_FAILURE);
        }

        random_traumdreieck(t);
        traumdreieck_speichern(t, dateiname);
    }
    
    traumdreieck_loeschen(t);
    
    return 0;
}
