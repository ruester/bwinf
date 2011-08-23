/* 29. Bundeswettbewerb Informatik - 2. Runde
 * Aufgabe 3 - Traumdreiecke
 *
 * Autor: Matthias Rüster
 *
 * E-Mail: ruester@molgen.mpg.de
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <unistd.h>

#include "util.h"

/* Beschreibung:
 *     kopiert die Farbwerte der Murmeln eines Traumdreiecks
 * Eingabewerte:
 *     1. Argument: Ziel des Kopiervorgangs
 *     2. Argument: Quelle des Kopiervorgangs
 * Rueckgabewerte:
 *     keine */
static void kopiere_murmeln(struct traumdreieck *dest,
                            struct traumdreieck *src)
{
    int i;

    for (i = 0 ; i < src->anzahl_ebenen; i++)
        memcpy(dest->murmel[i], src->murmel[i],
               (i + 1) * sizeof(unsigned char));
}

/* Beschreibung:
 *     - mutiert die Farbwerte der Murmeln an geeigneten Stellen
 *     - eine zufaellige Murmel eines gleichseitigen Dreiecks
 *       wird ein zufaelliger Farbwert zugeordnet
 * Eingabewert:
 *     Zeiger auf das Traumdreieck
 * Rueckgabewerte:
 *     keine */
static void gezielte_mutation(struct traumdreieck *t)
{
    int i, j, k, farbe;
    struct traumdreieck *kopie;

    /* Kopie des Traumdreiecks anlegen */
    kopie = neues_traumdreieck(t->anzahl_ebenen);

    /* Farbwerte der Murmeln kopieren */
    kopiere_murmeln(kopie, t);

    /* gleichseitige Dreiecke von Murmeln mit derselben Farbe suchen
       => dabei wird in der Kopie des Traumdreiecks gesucht
          und im Traumdreieck "t" mutiert */
    for (i = 1; i < t->anzahl_ebenen; i++)
        for (j = 0; j < i; j++)
            for (k = j + 1; k <= i; k++)
                /* auf gleiche Farbwerte pruefen */
                if (kopie->murmel[i][j] == kopie->murmel[i][k]
                    && kopie->murmel[i - k + j][j] == kopie->murmel[i][j]) {
                    /* zufaelligen Farbwert erzeugen */
                    farbe = 1 + rand() % t->anzahl_farben;

                    /* eine zufaellige Murmel des
                       gleichseitigen Dreiecks veraendern */
                    switch (rand() % 3) {
                    case 0:
                        t->murmel[i][j] = farbe;
                        break;

                    case 1:
                        t->murmel[i][k] = farbe;
                        break;

                    case 2:
                        t->murmel[i - k + j][j] = farbe;
                        break;
                    }
                }

    /* Speicher freigeben */
    traumdreieck_loeschen(kopie);
}

/* Beschreibung:
 *     zaehlt in einem Traumdreieck die gleichseitigen
 *     Dreiecke von Murmeln mit denselben Farbwerten
 * Eingabewert:
 *     Zeiger auf das Traumdreieck
 * Rueckgabewerte:
 *     keine */
static int bestimme_fitness(struct traumdreieck *t)
{
    int i, j, k, fitness;

    fitness = 0;

    for (i = 1; i < t->anzahl_ebenen; i++)
        for (j = 0; j < i; j++)
            for (k = j + 1; k <= i; k++)
                if (t->murmel[i][j] == t->murmel[i][k]
                    && t->murmel[i - k + j][j] == t->murmel[i][j])
                    /* ein gleichseitiges Dreieck von Murmeln
                       mit denselben Farben wurde gefunden */
                    fitness--;
    
    return fitness;
}

int main(int argc, char *argv[])
{
    FILE *datei;
    struct traumdreieck *t;
    char output[NAME_MAX];
    
    /* Parameter ueberpruefen */
    if (argc != 2) {
        fprintf(stderr, "Falsche Parameter\n");
        exit(EXIT_FAILURE);
    }
    
    srand(time(NULL) + getpid());
    
    /* Traumdreieck laden, dass mutiert werden soll */
    t = traumdreieck_laden(argv[1]);
    
    gezielte_mutation(t);
    
    /* mutiertes Traumdreieck speichern */
    traumdreieck_speichern(t, argv[1]);
    
    sprintf(output, "%d.fitness", atoi(argv[1]));
    
    /* .fitness-Datei anlegen */
    if ((datei = fopen(output, "w")) == NULL) {
        perror("perror");
        exit(EXIT_FAILURE);
    }
    
    /* Fitnesswert in die Datei schreiben */
    fprintf(datei, "%d\n", bestimme_fitness(t));

    /* Datei schliessen */
    if (fclose(datei) == EOF) {
        perror("fclose");
        exit(EXIT_FAILURE);
    }

    traumdreieck_loeschen(t);
    
    return 0;
}
