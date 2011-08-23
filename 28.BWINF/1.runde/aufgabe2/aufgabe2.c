/* Bundeswettbewerb Informatik
 * Aufgabe 2 - Handytasten
 *
 * Autor: Matthias Rüster
 *
 * E-Mail: ruester@molgen.mpg.de 
 */

#include <stdio.h>              /* fuer printf/scanf/fopen/... */
#include <stdlib.h>             /* fuer malloc/calloc/exit/... */
#include <string.h>             /* fuer strlen */

/* Konstanten definieren */
#define ANZAHL_BUCHSTABEN 26
#define ANZAHL_TASTEN 8
#define MAX_BUCHSTABEN_PRO_TASTE (ANZAHL_BUCHSTABEN - ANZAHL_TASTEN + 1)
#define LETZTE_TASTE (ANZAHL_TASTEN - 1)

struct Taste {
    /* Array der Buchstaben, die auf der Taste liegen */
    char buchstaben[MAX_BUCHSTABEN_PRO_TASTE];
};

struct Tastatur {
    /* Tastatur besteht aus Array von Tasten */
    struct Taste taste[ANZAHL_TASTEN];

    /* Kosten der Tastenbelegung */
    unsigned int kosten;
};

typedef unsigned int Haeufigkeiten;

/* Beschreibung:
 *     Funktion gibt die Tastenbelegung und die Kosten
 *     einer Tastatur auf den Bildschirm aus
 * Eingabewerte:
 *     t: - Zeiger auf Struktur Tastatur,
 *          die ausgegeben werden soll
 *        - Zeiger darf nicht NULL sein
 * Rueckgabewerte:
 *     keine Rueckgabewerte
 */
void tastenbelegung_ausgeben(struct Tastatur *t)
{
    int i;                      /* Zaehlvariable */

    /* alle Tasten durchgehen */
    for (i = 0; i < ANZAHL_TASTEN; i++)
        /* Buchstaben ausgeben */
        printf("Taste %d: %s\n", i + 2, t->taste[i].buchstaben);

    /* Kosten ausgeben */
    printf("Kosten der Tastenbelegung: %d\n", t->kosten);
}

/* Beschreibung:
 *     Funktion holt Speicher fuer ein Haeufigkeitenarray,
 *     liest die Haeufigkeitswerte der einzelnen Buchstaben aus
 *     einer Datei und schreibt diese in ein Haeufigkeitenarray
 * Eingabewerte:
 *     dateiname: Dateiname der Datei, die die Haeufigkeitswerte
 *                der einzelnen Buchstaben enthaelt
 * Rueckgabewerte:
 *     - Zeiger auf das Haeufigkeitenarray
 *     - bei Fehlern wird das Programm beendet
 */
Haeufigkeiten *lese_haeufigkeiten(char *dateiname)
{
    int i,                      /* Zaehlvariable */
     temp;                      /* zum Auslesen der Datei benoetigt */
    FILE *input;                /* Zeiger auf Datei mit den Haeufigkeitswerten */
    Haeufigkeiten *h;           /* Zeiger auf Haeufigkeitenarray */

    /* Datei oeffnen */
    if ((input = fopen(dateiname, "r")) == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    /* Speicher fuer das Haeufigkeitenarray holen */
    if ((h = malloc(sizeof(unsigned int) * ANZAHL_BUCHSTABEN)) == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    /* Einlesen der Haeufigkeitswerte bis das Ende der Datei erreicht ist */
    for (i = 0; fscanf(input, "%d\n", &temp) != EOF; i++) {
        if (i > ANZAHL_BUCHSTABEN - 1) {
            /* zu viele Buchstabenhaeufigkeiten */
            printf("Datei %s ungueltig (zu viele Werte)!\n", dateiname);
            exit(EXIT_FAILURE);
        }

        /* Haeufigkeitswert in das Haeufigkeitenarray eintragen */
        h[i] = temp;
    }

    if (i < ANZAHL_BUCHSTABEN) {
        /* zu wenig Buchstabenhaeufigkeiten */
        printf("Datei %s ungueltig (zu wenig Werte)!\n", dateiname);
        exit(EXIT_FAILURE);
    }

    /* Datei schliessen */
    fclose(input);

    /* Adresse des Haeufigkeitenarrays zurueckgeben */
    return (h);
}

/* Beschreibung: 
 *     Funktion berechnet die Kosten einer Tastenbelegung
 *     und traegt diese in t ein 
 * Eingabewerte:
 *     t: - Zeiger auf Struktur Tastatur,
 *          von der die Kosten berechnet werden sollen
 *        - Zeiger darf nicht NULL sein
 *     h: - das Haeufigkeitenarray, welches die
 *          Buchstabenhaeufigkeiten enthaelt
 *        - Zeiger darf nicht NULL sein
 * Rueckgabewerte:
 *     keine Rueckgabewerte
 */
void berechne_kosten(struct Tastatur *t, Haeufigkeiten * h)
{
    int i, j,                   /* Zaehlvariablen */
     kosten;                    /* zum Aufaddieren der einzelnen Kosten benoetigt */

    /* alle Tasten durchlaufen */
    for (i = 0, kosten = 0; i < ANZAHL_TASTEN; i++)
        /* die Buchstaben der Taste durchlaufen */
        for (j = 0; t->taste[i].buchstaben[j] != '\0'; j++)
            /* Haeufigkeitswert des Buchstaben 
             * mit dem Stellenwert multiplizieren */
            kosten += (j + 1) * h[t->taste[i].buchstaben[j] - 'A'];

    /* berechnete Kosten in die Tastatur eintragen */
    t->kosten = kosten;
}

/* Beschreibung:
 *     Funktion holt Speicher fuer eine Struktur Tastatur und
 *     erstellt fuer diese die Tastenbelegung, welche durch ein
 *     Werte-Array definiert ist
 * Eingabewerte:
 *     a: - das Werte-Array, welches definiert wie viele Buchstaben
 *          auf eine Taste zu platzieren sind
 *        - Zeiger darf nicht NULL sein
 * Rueckgabewerte:
 *     - Zeiger auf die Struktur Tastatur
 *     - bei Fehlern wird das Programm beendet
 */
struct Tastatur *erstelle_tastenbelegung(unsigned int *a)
{
    struct Tastatur *t;         /* Zeiger auf Tastatur */
    int i, j, merk;             /* Zaehlvariablen */

    /* Speicher fuer Tastatur holen */
    if ((t = calloc(1, sizeof(struct Tastatur))) == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    /* Buchstaben auf Tasten schreiben */
    for (i = 0, merk = 0; i < ANZAHL_TASTEN; i++)
        for (j = 0; j < a[i]; j++, merk++)
            t->taste[i].buchstaben[strlen(t->taste[i].buchstaben)] =
                merk + 'A';

    /* Adresse der Tastenbelegung zurueckgeben */
    return (t);
}

/* Beschreibung:
 *     Funktion findet die optimale Tastenbelegung anhand der 
 *     Haeufigkeiten der einzelnen Buchstaben
 * Eingabewerte:
 *     h: - Zeiger auf das Haeufigkeitenarray mit den Haeufigkeitswerten
 *          der einzelnen Buchstaben einer Sprache
 *        - Zeiger darf nicht NULL sein
 * Rueckgabewerte:
 *     - Zeiger auf die Struktur Tastatur mit
 *       der optimalen Tastaturbelegung
 *     - bei Fehlern wird das Programm beendet
 */
struct Tastatur *finde_optimale_tastenbelegung(Haeufigkeiten * h)
{
    struct Tastatur *optimal,   /* Zeiger auf Tastatur mit geringsten Kosten
                                 * und optimaler Tastenbelegung */
    *temp;                      /* Zeiger auf temporaere Tastatur */
    unsigned int i,             /* Zaehlvariable */
     a[ANZAHL_TASTEN],          /* Wert-Array (Anzahl der Buchstaben
                                 * auf den jeweiligen Tasten) */
     summe;                     /* Summe aller Werte */

    int j;                      /* Index eines Elements mit Ueberlauf */

    /* noch keine optimale Tastenbelegung gefunden */
    optimal = NULL;

    /* Tasten mit 1 initialisieren und Summen berechnen */
    for (i = 0; i < ANZAHL_TASTEN - 1; i++)
        a[i] = 1;

    /* Wert des letzten Elements festlegen */
    a[LETZTE_TASTE] = MAX_BUCHSTABEN_PRO_TASTE;

    /* alle Kombinationen durchgehen */
    while (1) {
        /* neue Kombination */
        temp = erstelle_tastenbelegung(a);

        /* Kosten der Tastenbelegung berechnen und eintragen */
        berechne_kosten(temp, h);

        /* Kosten vergleichen */
        if (optimal == NULL
            || (temp != NULL && temp->kosten < optimal->kosten)) {
            free(optimal);      /* vorher gemerkte Tastatur loeschen */
            optimal = temp;     /* Tastenbelegung merken (weil geringere Kosten) */
        } else
            free(temp);         /* Tastenbelegung ist nicht besser */

        /* naechste Kombination */
        a[LETZTE_TASTE - 1]++;  /* vorletztes Element erhoehen */
        a[LETZTE_TASTE]--;      /* letztes Element reduzieren */

        j = LETZTE_TASTE - 1;   /* Index des vorletzten Elements */

        /* auf Ueberlauf pruefen */
        while (a[LETZTE_TASTE] == 0) {
            a[j - 1]++;         /* Ueberlauf weiterleiten */
            a[j] = 1;           /* Element nach Ueberlaufstelle auf 1 setzen */

            j--;                /* Element links vom Ueberlauf betrachten */

            if (j < 0)          /* ein Ueberlauf beim ersten Element */
                return (optimal);       /* optimale Tastatur zurueckgeben */

            /* Summe neu berechnen */
            for (i = 0, summe = 0; i < ANZAHL_TASTEN; i++)
                summe += a[i];

            /* Berechnen des Wertes des letzten Elements */
            a[LETZTE_TASTE] = ANZAHL_BUCHSTABEN - summe;
        }
    }
}

/* Beschreibung:
 *     main-Funktion
 * Eingabewerte:
 *     argv: - Dateinamen der Datei mit den Haeufigkeitswerten
 *             der einzelnen Buchstaben einer Sprache
 * Rueckgabewerte:
 *     - bei erfolgreichem Durchlauf des Programms: EXIT_SUCCESS 
 *     - bei falscher Parameteruebergabe: EXIT_FAILURE
 */
int main(int argc, char *argv[])
{
    struct Tastatur *optimal;   /* zum Speichern der optimalen Tastatur */
    Haeufigkeiten *h;           /* Haeufigkeiten der einzelnen Buchstaben */

    /* Parameteruebergabe pruefen */
    if (argc < 2 || argc > 2) {
        printf("Bitte den Dateinamen der Datei mit den");
        printf(" Haeufigkeitswerten als Parameter angeben!\n");
        return (EXIT_FAILURE);
    }

    /* Einlesen der Haeufigkeitswerte aus der angegebenen Datei */
    h = lese_haeufigkeiten(argv[1]);

    /* optimale Tastenbelegung finden */
    optimal = finde_optimale_tastenbelegung(h);

    /* Ausgabe der optimalen Tastenbelegung */
    printf("optimale Tastenbelegung fuer %s:\n", argv[1]);
    tastenbelegung_ausgeben(optimal);

    /* Speicher freigeben */
    free(optimal);
    free(h);

    return (EXIT_SUCCESS);
}
