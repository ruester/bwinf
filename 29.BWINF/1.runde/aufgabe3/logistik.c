/* Bundeswettbewerb Informatik
 * Aufgabe 3 - Logistik
 *
 * Autor: Matthias Rüster
 *
 * E-Mail: ruester@molgen.mpg.de
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "logistik.h"

#define ANZAHL_STANDORTE 3
#define ANZAHL_TAGE      7

#define BREITE (ANZAHL_STANDORTE * (ANZAHL_STANDORTE - 1))

/* Indizes */
#define MONTAG     0
#define DIENSTAG   1
#define MITTWOCH   2
#define DONNERSTAG 3
#define FREITAG    4
#define SAMSTAG    5
#define SONNTAG    6
#define A  0
#define B  1
#define C  2
#define AB 0                    /* von A nach B */
#define AC 1
#define BA 2
#define BC 3
#define CA 4
#define CB 5

#define MAX(a, b) ((a > b) ? a : b)
#define MIN(a, b) ((a < b) ? a : b)

/* Beschreibung:
 *     holt Speicher fuer ein zweidimensionales Array
 * Eingabewerte:
 *     1. Argument: Hoehe des Arrays
 *     2. Argument: Breite des Arrays
 * Rueckgabewerte:
 *     Zeiger auf das zweidimensionale Array */
int **neues_array(int hoehe, int breite)
{
    int **n;
    int i;

    /* Speicher fuer die Zeilen holen */
    if ((n = calloc(hoehe, sizeof(int *))) == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    /* Speicher fuer die Spalten holen */
    for (i = 0; i < hoehe; i++)
        if ((n[i] = calloc(breite, sizeof(int))) == NULL) {
            perror("calloc");
            exit(EXIT_FAILURE);
        }

    /* Zeiger zurueckgeben */
    return (n);
}

/* Beschreibung:
 *     - erstellt ein neues Auftragsbuch (zweidimensionale Array)
 *     - liest die Daten fuer das Auftragsbuch aus einer Datei
 * Eingabewerte:
 *     Dateiname
 * Rueckgabewerte:
 *     Zeiger auf das Auftragsbuch */
int **auftragsbuch_erstellen(char *dateiname)
{
    FILE *datei;
    char zeile[LINE_MAX];
    int **auftragsbuch;
    int i;

    /* Datei oeffnen */
    if ((datei = fopen(dateiname, "r")) == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    /* Speicher holen */
    auftragsbuch = neues_array(ANZAHL_TAGE, BREITE);

    i = 0;

    /* Daten einlesen */
    while ((fgets(zeile, LINE_MAX, datei)) != NULL) {
        if (zeile[0] == '#')
            continue;           /* Kommentarzeile */

        if (i >= ANZAHL_TAGE) {
            fprintf(stderr, "Fehlerhaftes Auftragsbuch\n");
            exit(EXIT_FAILURE);
        }

        /* Daten speichern */
        if (sscanf(zeile, "%d %d %d %d %d %d\n",
                   &(auftragsbuch[i][AB]), &(auftragsbuch[i][AC]),
                   &(auftragsbuch[i][BA]), &(auftragsbuch[i][BC]),
                   &(auftragsbuch[i][CA]),
                   &(auftragsbuch[i][CB])) == EOF) {
            fprintf(stderr, "Fehlerhaftes Auftragsbuch\n");
            exit(EXIT_FAILURE);
        }

        i++;
    }

    /* Datei schliessen */
    fclose(datei);

    /* Zeiger zurueckgeben */
    return (auftragsbuch);
}

/* Beschreibung:
 *     gibt den Wochentag auf dem Bildschirm aus
 * Eingabewerte:
 *     Tag (0 = Montag, 1 = Dienstag, ...)
 * Rueckgabewerte:
 *     keine */
void print_wochentag(int tag)
{
    switch (tag) {
    case MONTAG:
        printf("Montag:     ");
        break;

    case DIENSTAG:
        printf("Dienstag:   ");
        break;

    case MITTWOCH:
        printf("Mittwoch:   ");
        break;

    case DONNERSTAG:
        printf("Donnerstag: ");
        break;

    case FREITAG:
        printf("Freitag:    ");
        break;

    case SAMSTAG:
        printf("Samstag:    ");
        break;

    case SONNTAG:
        printf("Sonntag:    ");
        break;
    }
}

/* Beschreibung:
 *     gibt ein Auftragsbuch aus
 * Eingabewerte:
 *     Zeiger auf das Auftragsbuch
 * Rueckgabewerte:
 *     keine */
void print_auftragsbuch(int **auftragsbuch)
{
    int i;

    printf("%12cA->B   A->C   B->A   B->C   C->A   C->B\n", ' ');

    for (i = MONTAG; i <= SONNTAG; i++) {
        print_wochentag(i);
        printf("%-6d %-6d %-6d %-6d %-6d %-6d\n",
               auftragsbuch[i][AB], auftragsbuch[i][AC],
               auftragsbuch[i][BA], auftragsbuch[i][BC],
               auftragsbuch[i][CA], auftragsbuch[i][CB]);
    }

    printf("\n");
}

/* Beschreibung:
 *     berechnet die Werte fuer die Tabellen anhand des Auftragsbuches
 * Eingabewerte:
 *     1. Agrument: Zeiger auf das Auftragsbuch
 *     2. Argument: Zeiger auf das Array mit den ankommenden Transportern
 *     3. Argument: Zeiger auf das Array mit den wegfahrenden Transportern
 *     4. Argument: Zeiger auf das Array mit den aktuellen Transportern
 *     5. Argument: Zeiger auf das Array der anzumietenden Transporter
 *     6. Argument: Zeiger auf das Array mit den ueberschuessigen Transportern
 * Rueckgabewerte:
 *     keine */
void berechne_tabellen(int **auftragsbuch,
                       int an[ANZAHL_TAGE][ANZAHL_STANDORTE],
                       int ab[ANZAHL_TAGE][ANZAHL_STANDORTE],
                       int transporter[ANZAHL_TAGE][ANZAHL_STANDORTE],
                       int mieten[ANZAHL_TAGE][ANZAHL_STANDORTE],
                       int ueber[ANZAHL_TAGE][ANZAHL_STANDORTE])
{
    int i;

    /* Anzahl der ankommenden und wegfahrenden Transporter bestimmen */
    for (i = MONTAG; i <= SONNTAG; i++) {
        ab[i][A] = auftragsbuch[i][AB] + auftragsbuch[i][AC];
        ab[i][B] = auftragsbuch[i][BA] + auftragsbuch[i][BC];
        ab[i][C] = auftragsbuch[i][CA] + auftragsbuch[i][CB];

        an[i][A] = auftragsbuch[i][BA] + auftragsbuch[i][CA];
        an[i][B] = auftragsbuch[i][AB] + auftragsbuch[i][CB];
        an[i][C] = auftragsbuch[i][AC] + auftragsbuch[i][BC];
    }

    for (i = MONTAG; i <= SONNTAG; i++) {
        if (i == MONTAG) {
            mieten[i][A] = ab[i][A];
            mieten[i][B] = ab[i][B];
            mieten[i][C] = ab[i][C];
            continue;
        }

        transporter[i][A] = transporter[i - 1][A] + an[i - 1][A]
            - ab[i - 1][A] + mieten[i - 1][A];
        transporter[i][B] = transporter[i - 1][B] + an[i - 1][B]
            - ab[i - 1][B] + mieten[i - 1][B];
        transporter[i][C] = transporter[i - 1][C] + an[i - 1][C]
            - ab[i - 1][C] + mieten[i - 1][C];
        
        mieten[i][A] = MAX(-transporter[i][A] + ab[i][A], 0);
        mieten[i][B] = MAX(-transporter[i][B] + ab[i][B], 0);
        mieten[i][C] = MAX(-transporter[i][C] + ab[i][C], 0);

        ueber[i][A] = MAX(transporter[i][A] - ab[i][A], 0);
        ueber[i][B] = MAX(transporter[i][B] - ab[i][B], 0);
        ueber[i][C] = MAX(transporter[i][C] - ab[i][C], 0);
    }
}

/* Beschreibung:
 *     - berechnet die Anzahl der Transporter die angemietet werden
 *       muessen, wenn keine Leerfahrten erlaubt sind
 *     - gibt die Ergebnisse auf dem Bildschirm aus
 * Eingabewerte:
 *     - Zeiger auf das Auftragsbuch
 * Rueckgabewerte:
 *     keine */
void ohne_leerfahrten(int **auftragsbuch)
{
    int an[ANZAHL_TAGE][ANZAHL_STANDORTE] = {{0}, {0}},
        ab[ANZAHL_TAGE][ANZAHL_STANDORTE] = {{0}, {0}},
        mieten[ANZAHL_TAGE][ANZAHL_STANDORTE] = {{0}, {0}},
        transporter[ANZAHL_TAGE][ANZAHL_STANDORTE] = {{0}, {0}},
        ueber[ANZAHL_TAGE][ANZAHL_STANDORTE] = {{0}, {0}};
    int i, summe_a, summe_b, summe_c;

    summe_a = summe_b = summe_c = 0;

    printf("ohne Leerfahrten:\n=================\n");

    berechne_tabellen(auftragsbuch, an, ab, transporter, mieten, ueber);
    
    /* Ergebnisse ausgeben */
    for (i = MONTAG; i <= SONNTAG; i++) {
        if (mieten[i][A]) {
            print_wochentag(i);
            printf("%4d Transporter fuer Standort A benoetigt\n",
                   mieten[i][A]);
        }

        if (mieten[i][B]) {
            print_wochentag(i);
            printf("%4d Transporter fuer Standort B benoetigt\n",
                   mieten[i][B]);
        }

        if (mieten[i][C]) {
            print_wochentag(i);
            printf("%4d Transporter fuer Standort C benoetigt\n",
                   mieten[i][C]);
        }

        summe_a += mieten[i][A];
        summe_b += mieten[i][B];
        summe_c += mieten[i][C];
    }

    printf("\nAm Ende der Woche:\n");
    printf("%4d Transporter am Standort A\n",
           transporter[SONNTAG][A]);
    printf("%4d Transporter am Standort B\n",
           transporter[SONNTAG][B]);
    printf("%4d Transporter am Standort C\n",
           transporter[SONNTAG][C]);

    printf("\nzu Beginn der Woche %d Transporter anmieten:\n",
           summe_a + summe_b + summe_c);
    printf("%4d Transporter fuer Standort A\n", summe_a);
    printf("%4d Transporter fuer Standort B\n", summe_b);
    printf("%4d Transporter fuer Standort C\n\n", summe_c);
}

/* Beschreibung:
 *     - berechnet die Anzahl der Transporter die angemietet werden
 *       muessen, wenn Leerfahrten erlaubt sind
 *     - gibt die Ergebnisse auf dem Bildschirm aus
 *     - aendert das Auftragsbuch zum neuen Tourenplan
 * Eingabewerte:
 *     Zeiger auf das Auftragsbuch
 * Rueckgabewerte:
 *     keine  */
void mit_leerfahrten(int **auftragsbuch)
{
    int leerfahrten[ANZAHL_TAGE][BREITE] = {{0}, {0}},
        leerfahrten_ausgabe[ANZAHL_TAGE][BREITE] = {{0}, {0}};
    int an[ANZAHL_TAGE][ANZAHL_STANDORTE] = {{0}, {0}},
        ab[ANZAHL_TAGE][ANZAHL_STANDORTE] = {{0}, {0}},
        ueber[ANZAHL_TAGE][ANZAHL_STANDORTE] = {{0}, {0}},
        mieten[ANZAHL_TAGE][ANZAHL_STANDORTE] = {{0}, {0}},
        transporter[ANZAHL_TAGE][ANZAHL_STANDORTE] = {{0}, {0}};
    int i, summe_a, summe_b, summe_c;

    berechne_tabellen(auftragsbuch, an, ab, transporter, mieten, ueber);

    printf("mit Leerfahrten:\n================\n");

    /* Leerfahrten berechnen */
    for (i = MONTAG; i <= SONNTAG; i++) {
        leerfahrten[i][AB] = MIN(ueber[i][A],
                                 mieten[i + 1][B]);

        leerfahrten[i][AC] = MIN(ueber[i][A] - leerfahrten[i][AB],
                                 mieten[i + 1][C]);

        leerfahrten[i][BA] = MIN(ueber[i][B],
                                 mieten[i + 1][A]);

        leerfahrten[i][BC] = MIN(ueber[i][B] - leerfahrten[i][BA],
                                 mieten[i + 1][C] - leerfahrten[i][AC]);

        leerfahrten[i][CA] = MIN(ueber[i][C],
                                 mieten[i + 1][A] - leerfahrten[i][BA]);

        leerfahrten[i][CB] = MIN(ueber[i][C] - leerfahrten[i][CA],
                                 mieten[i + 1][B] - leerfahrten[i][AB]);

        if (leerfahrten[i][AB]) {
            /* Tourenplan korrigieren */
            auftragsbuch[i][AB] += leerfahrten[i][AB];
            
            /* Tabellen aktualisieren */
            berechne_tabellen(auftragsbuch, an, ab, transporter, mieten,
                              ueber);

            /* Aenderung am Tourenplan merken */
            leerfahrten_ausgabe[i][AB] += leerfahrten[i][AB];

            /* den selben Tag nochmal auf Leerfahrten pruefen */
            i--;
            continue;
        }

        if (leerfahrten[i][AC]) {
            auftragsbuch[i][AC] += leerfahrten[i][AC];
            berechne_tabellen(auftragsbuch, an, ab, transporter, mieten,
                              ueber);

            leerfahrten_ausgabe[i][AC] += leerfahrten[i][AC];

            i--;
            continue;
        }

        if (leerfahrten[i][BA]) {
            auftragsbuch[i][BA] += leerfahrten[i][BA];
            berechne_tabellen(auftragsbuch, an, ab, transporter, mieten,
                              ueber);

            leerfahrten_ausgabe[i][BA] += leerfahrten[i][BA];

            i--;
            continue;
        }

        if (leerfahrten[i][BC]) {
            auftragsbuch[i][BC] += leerfahrten[i][BC];
            berechne_tabellen(auftragsbuch, an, ab, transporter, mieten,
                              ueber);

            leerfahrten_ausgabe[i][BC] += leerfahrten[i][BC];

            i--;
            continue;
        }

        if (leerfahrten[i][CA]) {
            auftragsbuch[i][CA] += leerfahrten[i][CA];
            berechne_tabellen(auftragsbuch, an, ab, transporter, mieten,
                              ueber);

            leerfahrten_ausgabe[i][CA] += leerfahrten[i][CA];

            i--;
            continue;
        }

        if (leerfahrten[i][CB]) {
            auftragsbuch[i][CB] += leerfahrten[i][CB];
            berechne_tabellen(auftragsbuch, an, ab, transporter, mieten,
                              ueber);

            leerfahrten_ausgabe[i][CB] += leerfahrten[i][CB];

            i--;
            continue;
        }
    }

    /* Ergebnisse ausgeben */
    for (i = MONTAG; i <= SONNTAG; i++) {
        if (mieten[i][A]) {
            print_wochentag(i);
            printf("%4d Transporter fuer Standort A benoetigt\n",
                   mieten[i][A]);
        }

        if (mieten[i][B]) {
            print_wochentag(i);
            printf("%4d Transporter fuer Standort B benoetigt\n",
                   mieten[i][B]);
        }

        if (mieten[i][C]) {
            print_wochentag(i);
            printf("%4d Transporter fuer Standort C benoetigt\n",
                   mieten[i][C]);
        }

        if (leerfahrten_ausgabe[i][AB]) {
            print_wochentag(i);
            printf("%4d Leerfahrten von A nach B\n",
                   leerfahrten_ausgabe[i][AB]);
        }

        if (leerfahrten_ausgabe[i][AC]) {
            print_wochentag(i);
            printf("%4d Leerfahrten von A nach C\n",
                   leerfahrten_ausgabe[i][AC]);
        }

        if (leerfahrten_ausgabe[i][BA]) {
            print_wochentag(i);
            printf("%4d Leerfahrten von B nach A\n",
                   leerfahrten_ausgabe[i][BA]);
        }

        if (leerfahrten_ausgabe[i][BC]) {
            print_wochentag(i);
            printf("%4d Leerfahrten von B nach C\n",
                   leerfahrten_ausgabe[i][BC]);
        }

        if (leerfahrten_ausgabe[i][CA]) {
            print_wochentag(i);
            printf("%4d Leerfahrten von C nach A\n",
                   leerfahrten_ausgabe[i][CA]);
        }

        if (leerfahrten_ausgabe[i][CB]) {
            print_wochentag(i);
            printf("%4d Leerfahrten von C nach B\n",
                   leerfahrten_ausgabe[i][CB]);
        }

        summe_a += mieten[i][A];
        summe_b += mieten[i][B];
        summe_c += mieten[i][C];
    }

    printf("\nAm Ende der Woche:\n");
    printf("%4d Transporter am Standort A\n",
           transporter[SONNTAG][A]);
    printf("%4d Transporter am Standort B\n",
           transporter[SONNTAG][B]);
    printf("%4d Transporter am Standort C\n",
           transporter[SONNTAG][C]);

    printf("\nzu Beginn der Woche %d Transporter anmieten:\n",
           summe_a + summe_b + summe_c);
    printf("%4d Transporter fuer Standort A\n", summe_a);
    printf("%4d Transporter fuer Standort B\n", summe_b);
    printf("%4d Transporter fuer Standort C\n\n", summe_c);
}

/* Beschreibung:
 *     gibt den Speicher eines zweidimensionalen Arrays frei
 * Eingabewerte:
 *     - 1. Argument: Zeiger auf das Array
 *     - 2. Argument: Hoehe des Arrays
 * Rueckgabewerte:
 *     keine */
void array_freigeben(int **a, int hoehe)
{
    int i;

    for (i = 0; i < hoehe; i++)
        free(a[i]);

    free(a);
}

/* Beschreibung:
 *     gibt den Speicher eines Auftragsbuches frei
 * Eingabewerte:
 *     Zeiger auf das Auftragsbuch
 * Rueckgabewerte:
 *     keine */
void auftragsbuch_freigeben(int **auftragsbuch)
{
    array_freigeben(auftragsbuch, ANZAHL_TAGE);
}
