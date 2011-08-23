/* 29. Bundeswettbewerb Informatik - 2. Runde
 * Aufgabe 3 - Traumdreiecke
 *
 * Autor: Matthias Rüster
 *
 * E-Mail: ruester@molgen.mpg.de
 */

#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "util.h"

/* Beschreibung:
 *     erstellt ein neues Traumdreieck mit n Ebenen
 * Eingabewert:
 *     Anzahl der Ebenen
 * Rueckgabewert:
 *     Zeiger auf das Traumdreieck */
struct traumdreieck *neues_traumdreieck(int n)
{
    struct traumdreieck *t;
    int i;

    /* Speicher fuer die Struktur holen */
    if ((t = calloc(1, sizeof(struct traumdreieck))) == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    /* Speicher fuer die Murmeln holen */
    if ((t->murmel = calloc(n, sizeof(unsigned char *))) == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < n; i++) {
        if ((t->murmel[i] = calloc(n + 1, sizeof(unsigned char))) == NULL) {
            perror("calloc");
            exit(EXIT_FAILURE);
        }
    }

    /* Traumdreieck initialisieren */
    t->anzahl_ebenen = n;
    if (n < 9)
        t->anzahl_farben = ceil(sqrt(n));
    else
        t->anzahl_farben = floor(sqrt(n + 1));

    return t;
}

/* Beschreibung:
 *     gibt den Speicher eines Traumdreiecks frei
 * Eingabewert:
 *     Zeiger auf das Traumdreieck
 * Rueckgabewerte:
 *     keine */
void traumdreieck_loeschen(struct traumdreieck *t)
{
    int i;

    for (i = 0; i < t->anzahl_ebenen; i++)
        free(t->murmel[i]);

    free(t->murmel);
    free(t);
}

/* Beschreibung:
 *     speichert ein Traumdreieck als Datei ab
 * Einagbewerte:
 *     1. Argument: Zeiger auf das Traumdreieck
 *     2. Argument: Dateiname
 * Rueckgabewerte:
 *     keine */
void traumdreieck_speichern(struct traumdreieck *t, char *dateiname)
{
    int i;
    FILE *datei;
    
    /* Datei oeffnen */
    if ((datei = fopen(dateiname, "w")) == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    
    /* Anzahl der Ebenen des Traumdreiecks in die Datei schreiben */
    if (fwrite(&(t->anzahl_ebenen), sizeof(int), 1, datei) == EOF) {
        perror("fwrite");
        exit(EXIT_FAILURE);
    }
    
    /* Daten des Traumdreiecks in die datei schreiben */
    for (i = 0; i < t->anzahl_ebenen; i++) {
        if (fwrite(t->murmel[i], sizeof(unsigned char), i + 1, datei) == EOF) {
            perror("fwrite");
            exit(EXIT_FAILURE);
        }
    }
    
    /* Datei schliessen */
    if (fclose(datei) == EOF) {
        perror("fclose");
        exit(EXIT_FAILURE);
    }
}

/* Beschreibung:
 *     laedt ein Traumdreieck aus einer Datei
 * Eingabewert:
 *     Dateiname
 * Rueckgabewert:
 *     Zeiger auf das geladene Traumdreieck */
struct traumdreieck *traumdreieck_laden(char *dateiname)
{
    struct traumdreieck *t;
    FILE *datei;
    int i, n;
    
    /* Datei oeffnen */
    if ((datei = fopen(dateiname, "r")) == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    
    /* Anzahl der Ebenen des Traumdreieck einlesen */
    if ((fread(&n, sizeof(int), 1, datei)) == EOF) {
        perror("fread");
        exit(EXIT_FAILURE);
    }
    
    t = neues_traumdreieck(n);
    
    /* Daten des Traumdreiecks aus der Datei lesen */
    for (i = 0; i < n; i++) {
        if (fread(t->murmel[i], sizeof(unsigned char), i + 1, datei) == EOF) {
            perror("fread");
            exit(EXIT_FAILURE);
        }
    }
    
    /* Datei schliessen */
    if (fclose(datei) == EOF) {
        perror("fclose");
        exit(EXIT_FAILURE);
    }
    
    return t;
}

/* Beschreibung:
 *     zaehlt die verwendeten Farben im einen Traumdreieck
 * Eingabewert:
 *     Zeiger auf das Traumdreieck
 * Rueckgabewerte:
 *     Anzahl der verwendeten Farben */
static int benoetigte_farben(struct traumdreieck *t)
{
    int i, j, benoetigt;
    int farben[1000] = { 0 };

    for (i = 0; i < t->anzahl_ebenen; i++)
        for (j = 0; j <= i; j++)
            farben[t->murmel[i][j]] = 1;

    benoetigt = 0;
    
    for (i = 0; i < 1000; i++)
        if (farben[i])
            benoetigt++;

    return benoetigt;
}

/* Beschreibung:
 *     speichert ein Traumdreieck als eine Datei ab
 *     (lesbar, also keine Binaerdaten)
 * Eingabewerte:
 *     1. Argument: Zeiger auf das Traumdreieck
 *     2. Argument: Dateiname
 * Rueckgabewerte:
 *     keine */
void loesung_speichern(struct traumdreieck *t, char *dateiname)
{
    char b;
    int i, j;
    FILE *datei;
    
    /* Datei oeffnen */
    if ((datei = fopen(dateiname, "w")) == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    
    /* 'Ueberschrift' in Datei schreiben */
    if (fprintf(datei, "Traumdreieck fuer n = %d:\n", t->anzahl_ebenen) < 0) {
        perror("fprintf");
        exit(EXIT_FAILURE);
    }
    
    b = (t->anzahl_farben >= 10) ? 1 : 0;
    
    /* Daten des Traumdreiecks in die Datei schreiben */
    for (i = 0; i < t->anzahl_ebenen; i++) {
        for (j = 0; j <= i; j++) {
             if (fprintf(datei, "%*d%c", b ? 2 : 1, t->murmel[i][j], (i == j) ? '\n': ' ') == EOF) {
                 perror("fprintf");
                 exit(EXIT_FAILURE);
             }
        }
    }
    
    /* benoetigte Farben in Datei schreiben */
    if (fprintf(datei, "Anzahl benoetigter Farben: %d\n", benoetigte_farben(t)) == EOF) {
        perror("fprintf");
        exit(EXIT_FAILURE);
    }
    
    /* Datei schliessen */
    if (fclose(datei) == EOF) {
        perror("fclose");
        exit(EXIT_FAILURE);
    }
}
