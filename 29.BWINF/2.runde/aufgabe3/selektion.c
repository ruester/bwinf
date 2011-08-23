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

#include "util.h"

/* Beschreibung:
 *     kopiert eine Datei, die ein Traumdreieck enthaelt
 * Eingabewerte:
 *     1. Argument: Eingabedatei
 *     2. Argument: Ausgabedatei
 * Rueckgabewerte>
 *     keine */
static void copy(char *von, char *nach)
{
    struct traumdreieck *t;
    
    t = traumdreieck_laden(von);
    
    traumdreieck_speichern(t, nach);
    
    traumdreieck_loeschen(t);
}

int main(int argc, char *argv[])
{
    FILE *datei;
    int p, i, max, best1, best2;
    int *fitness;
    char dateiname[NAME_MAX];
    struct traumdreieck *t;

    /* Parameter pruefen */
    if (argc != 2 || (p = atoi(argv[1])) < 2) {
        fprintf(stderr, "Falsche Parameter\n");
        exit(EXIT_FAILURE);
    }

    /* Array fuer Fitnesswerte anlegen */
    if ((fitness = calloc(p, sizeof(int))) == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    /* .fitness-Dateien durchlaufen */
    for (i = 1; i <= p; i++) {
        sprintf(dateiname, "%d.fitness", i);
        
        /* Datei oeffnen */
        if ((datei = fopen(dateiname, "r")) == NULL) {
            perror("fopen");
            exit(EXIT_FAILURE);
        }
        
        /* Fitnesswert einlesen */
        if (fscanf(datei, "%d", &(fitness[i - 1])) == EOF) {
            perror("fscanf");
            exit(EXIT_FAILURE);
        }

        /* Datei schliessen */
        if (fclose(datei) == EOF) {
            perror("fclose");
            exit(EXIT_FAILURE);
        }

        /* pruefen, ob eine Loesung gefunden wurde */
        if (fitness[i - 1] == 0) {
            sprintf(dateiname, "%d.td", i);
            
            t = traumdreieck_laden(dateiname);
            
            loesung_speichern(t, "td.loesung");
            
            traumdreieck_loeschen(t);
            
            free(fitness);
            
            return 0;
        }
    }
    
    best1 = -1;
    best2 = -1;
    max = -INT_MAX;
    
    /* bestes Traumdreieck herausfinden */
    for (i = 0; i < p; i++)
        if (max < fitness[i]) {
            best1 = i;
            max = fitness[i];
        }
    
    /* beste Fitness ausgeben */
    if (isatty(STDOUT_FILENO)) {
        printf("%-10d\r", max);
        fflush(stdout);
    }
    
    max = -INT_MAX;
    
    /* zweitbestes Traumdreieck herausfinden */
    for (i = 0; i < p; i++)
        if (max < fitness[i] && i != best1) {
            best2 = i;
            max = fitness[i];
        }
    
    if (best1 == -1 || best2 == -1) {
        fprintf(stderr, "unerwarteter Fehler\n");
        exit(EXIT_FAILURE);
    }
    
    free(fitness);
    
    /* die beiden besten Traumdreiecke in die Dateien
       1.sel und 2.sel kopieren */
    sprintf(dateiname, "%d.td", best1 + 1);
    copy(dateiname, "1.sel");
    
    sprintf(dateiname, "%d.td", best2 + 1);
    copy(dateiname, "2.sel");
    
    return 0;
}
