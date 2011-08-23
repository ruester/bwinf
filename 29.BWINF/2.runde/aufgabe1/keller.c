/* 29. Bundeswettbewerb Informatik - 2. Runde
 * Aufgabe 1 - Kisten in Kisten in Kisten
 *
 * Autor: Matthias Rüster
 *
 * E-Mail: ruester@molgen.mpg.de
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#include "keller.h"
#include "kiste.h"

#define SWAP(a, b, typ) { typ h = a; a = b; b = h; }

/* Beschreibung:
 *     - summiert das Volumen aller Kisten im Keller auf
 *     - das Ergebnis wird in der Kellerstruktur gespeichert
 * Eingabewert:
 *     Zeiger auf die Kellerstruktur
 * Rueckgabewerte:
 *     keine */
static void berechne_gesamt_volumen(struct keller *k)
{
    int i;

    k->gesamt_volumen = 0L;

    for (i = 0; i < k->anzahl_kisten; i++)
        k->gesamt_volumen += k->kiste[i]->volumen;
}

/* Beschreibung:
 *     - summiert das Volumen aller nicht verpackten Kisten im Keller auf
 *     - das Ergebnis wird in der Kellerstruktur gespeichert
 * Eingabewert:
 *     Zeiger auf die Kellerstruktur
 * Rueckgabewerte:
 *     keine */
static void berechne_aeusseres_volumen(struct keller *k)
{
    int i;

    k->aeusseres_volumen = 0L;

    for (i = 0; i < k->anzahl_kisten; i++)
        if (!k->kiste[i]->verpackt)
            k->aeusseres_volumen += k->kiste[i]->volumen;
}

/* Beschreibung:
 *     Hilfsfunktion fuer das Ausgeben der Anleitung
 * Eingabewert:
 *     Zeiger auf die Kistenstruktur
 * Rueckgabewerte:
 *     keine */
static void print_rekursiv(struct kiste *k)
{
    if (k == NULL)
        return;

    if (k->inhalt[0] != NULL && k->inhalt[1] != NULL) {
        print_rekursiv(k->inhalt[0]);
        print_rekursiv(k->inhalt[1]);
        printf("Kiste %d (%3d, %3d, %3d) und Kiste %d (%3d, %3d, %3d) "
               "in Kiste %d (%3d, %3d, %3d) packen\n",
               k->inhalt[0]->index + 1, k->inhalt[0]->l, k->inhalt[0]->b,
               k->inhalt[0]->h, k->inhalt[1]->index + 1, k->inhalt[1]->l,
               k->inhalt[1]->b, k->inhalt[1]->h, k->index + 1, k->l, k->b,
               k->h);
        return;
    }

    if (k->inhalt[0] != NULL) {
        print_rekursiv(k->inhalt[0]);
        printf("Kiste %d (%3d, %3d, %3d) in "
               "Kiste %d (%3d, %3d, %3d) packen\n",
               k->inhalt[0]->index + 1, k->inhalt[0]->l, k->inhalt[0]->b,
               k->inhalt[0]->h, k->index + 1, k->l, k->b, k->h);
    }
}

/* Beschreibung:
 *     gibt die Anleitung zum Packen der Kisten auf dem Bildschirm aus
 * Eingabewert:
 *     Zeiger auf die Kellerstruktur
 * Rueckgabewerte:
 *     keine */
void print_anleitung(struct keller *k)
{
    int i, gesamt;

    if (k->aeusseres_volumen == k->gesamt_volumen) {
        printf("Es koennen keine Kisten verpackt werden\n\n");
        return;
    }

    gesamt = 0;

    berechne_aeusseres_volumen(k);

    for (i = 0; i < k->anzahl_kisten; i++) {
#ifdef DEBUG
        if (k->kiste[i]->inhalt[0] == NULL
            && k->kiste[i]->inhalt[1] == NULL)
            printf("Kiste %d (%3d, %3d, %3d) bleibt leer\n",
                   k->kiste[i]->index + 1, k->kiste[i]->l, k->kiste[i]->b,
                   k->kiste[i]->h);
#endif

        if (!k->kiste[i]->verpackt)
            print_rekursiv(k->kiste[i]);
    }

    printf("Gesamtvolumen der aeusseren Kisten: %lu (statt %lu)\n",
           k->aeusseres_volumen, k->gesamt_volumen);

    printf("Kompression: %.2f\n\n",
           (double) k->gesamt_volumen / (double) k->aeusseres_volumen);
}

/* Beschreibung:
 *     packt alle Kisten aus
 * Eingabewert:
 *     Zeiger auf die Kellerstruktur
 * Rueckgabewerte:
 *     keine */
static void reset_keller(struct keller *k)
{
    int i;

    for (i = 0; i < k->anzahl_kisten; i++) {
        k->kiste[i]->inhalt[0] = NULL;
        k->kiste[i]->inhalt[1] = NULL;
        k->kiste[i]->verpackt = 0;
    }

    berechne_aeusseres_volumen(k);
}

/* Beschreibung:
 *     liest die Kisten aus einer Datei
 * Eingabewerte:
 *     1. Argument: Zeiger auf die Kellerstruktur
 *     2. Argument: Zeiger auf den Dateinamen (String)
 * Rueckgabewerte:
 *     keine */
static void kisten_einlesen(struct keller *k, char *dateiname)
{
    int l, b, h, i;
    FILE *datei;
    char zeile[LINE_MAX] = { 0 };
    char ungueltig;

    if ((datei = fopen(dateiname, "r")) == NULL) {
        keller_loeschen(k);
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    while (memset(zeile, 0, sizeof(char) * LINE_MAX)
           && fgets(zeile, LINE_MAX, datei) != NULL) {
        if (zeile[0] == '#')
            continue;

        ungueltig = 0;

        for (i = 0; i < LINE_MAX; i++)
            if ((zeile[i] < '0' || zeile[i] > '9') && zeile[i] > ' ')
                ungueltig = 1;

        if (ungueltig) {
            printf("ungueltige Kiste: %s", zeile);
            continue;
        }

        if (sscanf(zeile, "%d %d %d", &l, &b, &h) == 3) {
            if (l < 2 || b < 2 || h < 2) {
                fprintf(stderr, "ungueltige Kiste: %3d %3d %3d\n", l, b, h);
                continue;
            }

            k->anzahl_kisten++;

            if ((k->kiste =
                 realloc(k->kiste,
                         k->anzahl_kisten
                         * sizeof(struct kiste *))) == NULL) {
                perror("realloc");
                exit(EXIT_FAILURE);
            }

            k->kiste[k->anzahl_kisten - 1] =
                neue_kiste(l, b, h, k->anzahl_kisten - 1);
        }
    }

    fclose(datei);
}

/* Beschreibung:
 *     setzt die Indizes der Kisten
 * Eingabewert:
 *     Zeiger auf die Kellerstruktur
 * Rueckgabewerte:
 *     keine */
static void setze_indizes(struct keller *k)
{
    int i;

    for (i = 0; i < k->anzahl_kisten; i++)
        k->kiste[i]->index = i;
}

/* Beschreibung:
 *     sortiert die Kisten nach ihren Volumen
 * Eingabewert:
 *     Zeiger auf die Kellerstruktur
 * Rueckgabewerte:
 *     keine */
static void sortiere_kisten(struct keller *k)
{
    int i, j, min;
    struct kiste *h;

    /* Hilfsvariable */
    h = neue_kiste(0, 0, 0, 0);

    /* Selectionsort */
    for (i = 0; i < k->anzahl_kisten - 1; i++) {
        min = i;

        for (j = i + 1; j < k->anzahl_kisten; j++)
            if (k->kiste[j]->volumen < k->kiste[min]->volumen)
                min = j;

        if (min != i) {
            memcpy(h, k->kiste[min], sizeof(struct kiste));
            memcpy(k->kiste[min], k->kiste[i], sizeof(struct kiste));
            memcpy(k->kiste[i], h, sizeof(struct kiste));
        }
    }

    free(h);
}

/* Beschreibung:
 *     erstellt eine neue Kellerstruktur anhand einer Eingabedatei
 * Eingabewert:
 *     Zeiger auf den Dateinamen (String)
 * Rueckgabewert:
 *     Zeiger auf die erstellte Kellerstruktur */
struct keller *neuer_keller_von_datei(char *dateiname)
{
    struct keller *k;

    if ((k = calloc(1, sizeof(struct keller))) == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    kisten_einlesen(k, dateiname);
    sortiere_kisten(k);
    setze_indizes(k);

    berechne_gesamt_volumen(k);
    berechne_aeusseres_volumen(k);

    return k;
}

/* Beschreibung:
 *     erstellt eine neue Kellerstruktur
 * Eingabewert:
 *     Anzahl der Kisten
 * Rueckgabewert:
 *     Zeiger auf die erstellte Kellerstruktur */
static struct keller *neuer_keller(int anzahl_kisten)
{
    int i;
    struct keller *k;

    if ((k = calloc(1, sizeof(struct keller))) == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    k->anzahl_kisten = anzahl_kisten;

    if ((k->kiste =
         calloc(k->anzahl_kisten, sizeof(struct kiste *))) == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < k->anzahl_kisten; i++)
        k->kiste[i] = neue_kiste(-1, -1, -1, -1);

    return k;
}

/* Beschreibung:
 *     gibt den Speicher einer Kellerstruktur wieder frei
 * Eingabewert:
 *     Zeiger auf die Kellerstruktur
 * Rueckgabewerte:
 *     keine */
void keller_loeschen(struct keller *k)
{
    int i;

    for (i = 0; i < k->anzahl_kisten; i++)
        free(k->kiste[i]);

    free(k->kiste);
    free(k);
}

/* Beschreibung:
 *     kopiert alle Daten einer Kellerstruktur
 * Eingabewerte:
 *     1. Argument: Zeiger auf die Kellerstruktur, die kopiert werden soll
 *     2. Argument: Zeiger auf die Kellerstruktur (Ziel des Kopiervorgangs)
 * Rueckgabewerte:
 *     keine */
void keller_kopieren(struct keller *von, struct keller *nach)
{
    int i;

    reset_keller(nach);

    nach->anzahl_kisten = von->anzahl_kisten;
    nach->aeusseres_volumen = von->aeusseres_volumen;
    nach->gesamt_volumen = von->gesamt_volumen;

    /* Kisten in der Kopie packen */
    for (i = 0; i < von->anzahl_kisten; i++) {
        kiste_kopieren(von->kiste[i], nach->kiste[i]);

        if (von->kiste[i]->inhalt[0] != NULL)
            nach->kiste[i]->inhalt[0] =
                nach->kiste[von->kiste[i]->inhalt[0]->index];
        if (von->kiste[i]->inhalt[1] != NULL)
            nach->kiste[i]->inhalt[1] =
                nach->kiste[von->kiste[i]->inhalt[1]->index];
    }
}

/* Beschreibung:
 *     erstellt eine Kopie von einer Kellerstruktur
 * Eingabewert:
 *     Zeiger auf die Kellerstruktur
 * Rueckgabewert:
 *     Zeiger auf die erstellte Kellerstruktur */
struct keller *kopie_von_keller(struct keller *k)
{
    struct keller *kopie;

    kopie = neuer_keller(k->anzahl_kisten);

    keller_kopieren(k, kopie);

    return kopie;
}

/* Beschreibung:
 *     gibt die Kisten einer Kellerstruktur auf dem Bildschirm aus
 * Eingabewert:
 *     Zeiger auf die Kellerstruktur
 * Rueckgabewerte:
 *     keine */
void print_kisten(struct keller *k)
{
    int i;

    printf("\nKisten:\n=======\n");

    for (i = 0; i < k->anzahl_kisten; i++)
        printf("Kiste %d: (%3d, %3d, %3d)\n", k->kiste[i]->index + 1,
               k->kiste[i]->l, k->kiste[i]->b, k->kiste[i]->h);

    printf("\n");
}

/* Beschreibung:
 *     - prueft jede Kiste, ob diese in einer
 *       anderen Kiste gepackt werden kann
 *     - sobald eine Kiste in eine andere Kiste passt, wird diese verpackt
 *     - es werden nie zwei Kisten in eine andere Kiste gepackt
 * Eingabewert:
 *     Zeiger auf die Kellerstruktur
 * Rueckgabewerte:
 *     keine */
static void packe_schnell_nur_einzeln(struct keller *k)
{
    int i, j;

    /* jede Kiste mit jeder kombinieren */
    for (i = 0; i < k->anzahl_kisten; i++)
        for (j = 0; j < k->anzahl_kisten; j++) {
            if (i == j)
                continue;

            if (kann_gepackt_werden(k->kiste[i], k->kiste[j])) {
                kiste_packen(k->kiste[i], k->kiste[j]);
                break; /* naechste Kiste verpacken */
            }
        }
}

/* Beschreibung:
 *     - prueft alle moeglichen Paare von Kisten, ob sie in einer
 *       anderen Kiste gepackt werden koennen
 *     - sobald zwei Kisten in eine andere Kiste passen, werden sie verpackt
 *     - danach werden die uebrigen Kisten mit der
 *       Funktion packe_schnell_nur_einzeln verpackt
 * Eingabewert:
 *     Zeiger auf die Kellerstruktur
 * Rueckgabewerte:
 *     keine */
static void packe_schnell(struct keller *k)
{
    int i, j, ziel;
    char b;

    /* Paare bilden */
    for (i = 0; i < k->anzahl_kisten; i++) {
        b = 0;

        for (j = i + 1; j < k->anzahl_kisten && !b; j++)
            for (ziel = 0; ziel < k->anzahl_kisten; ziel++) {
                if (ziel == i || ziel == j)
                    continue;

                if (koennen_gepackt_werden(k->kiste[i], k->kiste[j],
                                           k->kiste[ziel])) {
                    kisten_packen(k->kiste[i], k->kiste[j],
                                  k->kiste[ziel]);
                    b = 1; /* naechstes Paar bilden */
                    break;
                }
            }
    }

    packe_schnell_nur_einzeln(k);
}

/* Beschreibung:
 *     - findet schnell eine Moeglichkeit die Kisten zu packen
 *     - packt die Kisten nach 2 Methoden und vergleicht die Ergebnisse
 *     - die Bessere der beiden Loesungen wird auf dem Bildschirm ausgegeben
 * Eingabewert:
 *     Zeiger auf die Kellerstruktur
 * Rueckgabewerte:
 *     1 - Methode A war besser
 *     0 - Methode B war besser */
char finde_loesung_schnell(struct keller *k)
{
    struct keller *kopie;

    reset_keller(k);

    kopie = kopie_von_keller(k);

    /* beide Packmethoden ausprobieren */
    packe_schnell(k);
    packe_schnell_nur_einzeln(kopie);

    berechne_aeusseres_volumen(k);
    berechne_aeusseres_volumen(kopie);

    /* bessere Loesung herausfinden */
    if (kopie->aeusseres_volumen <= k->aeusseres_volumen) {
        keller_kopieren(kopie, k);
        keller_loeschen(kopie);
        return 1;
    }

    keller_loeschen(kopie);
    return 0;
}

/* Beschreibung:
 *     - durchlaeuft alle Moeglichkeiten bestimmte Kisten zu packen
 *     - durch das 3. und 4. Argument werden die Kisten festgelegt,
 *       die in Betracht gezogen werden
 * Eingabewerte:
 *     1. Argument: Zeiger auf die Kellerstruktur
 *     2. Argument: Zeiger auf die Kellerstruktur,
 *                  in der das Ergebnis gespeichert wird
 *     3. und 4. Argument: Start- und Endindex der zu untersuchenden Kisten
 *     5. und 6. Argument: Variablen zum Ueberspringen von doppelten Loesungen
 * Rueckgabewerte:
 *     keine */
static void bruteforce_rekursion_von_bis(struct keller *k,
                                         struct keller *min,
                                         int von, int bis,
                                         char skip, int skip2)
{
    int i, j, ziel;
    char aenderung;

    aenderung = 0;

    if (!skip) {
        for (i = von; i <= bis; i++)
            for (j = von; j <= bis; j++) {
                if (i == j)
                    continue;

                if (kann_gepackt_werden(k->kiste[i], k->kiste[j])) {
                    kiste_packen(k->kiste[i], k->kiste[j]);

                    bruteforce_rekursion_von_bis(k, min, von, bis, 0, 0);

                    kiste_auspacken(k->kiste[i], k->kiste[j]);

                    /* es wurde mindestens eine Kiste verpackt */
                    aenderung = 1;
                }
            }
    }

    for (i = von; i <= bis - 1; i++)
        for (j = i + 1; j <= bis; j++)
            for (ziel = von; ziel <= bis; ziel++) {
                if (j == ziel || i == ziel)
                    continue;

                if (koennen_gepackt_werden(k->kiste[i], k->kiste[j],
                                           k->kiste[ziel])) {
                    kisten_packen(k->kiste[i], k->kiste[j], k->kiste[ziel]);

                    bruteforce_rekursion_von_bis(k, min, von, bis, 1, i);

                    kisten_auspacken(k->kiste[i], k->kiste[j], k->kiste[ziel]);

                    /* es wurde mindestens eine Kiste verpackt */
                    aenderung = 1;
                }
            }

    /* es konnte keine Kisten mehr verpackt werden */
    if (!aenderung) {
        berechne_aeusseres_volumen(k);

        /* Loesung speichern, wenn Aussenvolumen geringer ist */
        if (k->aeusseres_volumen < min->aeusseres_volumen)
            keller_kopieren(k, min);
    }
}

/* Beschreibung:
 *     - sucht nach einer besseren Loesung,
 *       indem fuer je 7 Kisten eine Brute-Force-Suche durchgefuehrt wird
 *     - die Anleitung zum Packen der Kisten wird nur dann ausgegeben,
 *       wenn eine bessere Loesung gefunden wurde
 * Eingabewert:
 *     Zeiger auf die Kellerstruktur
 * Rueckgabewerte:
 *     keine */
void finde_loesung_quickbruteforce(struct keller *k)
{
    int i, anzahl_bruteforce, anzahl_kisten_bruteforce, von, bis;
    struct keller *min;

    min = kopie_von_keller(k);

    reset_keller(k);
    reset_keller(min);

    berechne_aeusseres_volumen(min);

    if (k->anzahl_kisten <= 7) {
        bruteforce_rekursion_von_bis(k, min, 0, k->anzahl_kisten - 1, 0, 0);
        keller_kopieren(min, k);
        keller_loeschen(min);
        return;
    }

    /* fuer je 7 Kisten den Brute-Force durchfuehren */
    anzahl_kisten_bruteforce = 7;

    /* Anzahl der durchzufuehrenden Brute-Forces berechnen */
    anzahl_bruteforce =
        (k->anzahl_kisten - 1) / (anzahl_kisten_bruteforce - 1);

    for (i = 0; i <= anzahl_bruteforce; i++) {
        von = i * (anzahl_kisten_bruteforce - 1);

        if (i == anzahl_bruteforce)
            bis = k->anzahl_kisten - 1; /* letzte Rekursion */
        else
            bis = von + anzahl_kisten_bruteforce - 1;

        /* Loesung verbessern */
        bruteforce_rekursion_von_bis(k, min, von, bis, 0, 0);

        /* bessere Loesung nach k kopieren */
        keller_kopieren(min, k);
    }

    /* eventuell noch nicht verpackte Kisten mit
       den beiden schnellen Methoden verpacken
       und danach pruefen welche Methode besser war */
    packe_schnell(k);
    packe_schnell_nur_einzeln(min);

    berechne_aeusseres_volumen(k);
    berechne_aeusseres_volumen(min);

    /* bessere Loesung herausfinden */
    if (min->aeusseres_volumen < k->aeusseres_volumen)
        keller_kopieren(min, k);

    keller_loeschen(min);
}

/* Beschreibung:
 *     - sortiert die 4 Arrays nach dem Restvolumen (aufsteigend)
 *     - ermittelt die neue untere bzw. obere Grenze fuer den Quicksort
 * Eingabewerte:
 *     1. - 4. Argument: die zu sortierenden Arrays
 *     5. Argument: untere Grenze
 *     6. Argument: obere Grenze
 * Rueckgabewerte:
 *     Index des neuen Teilerelements */
static int sortiere(int *von, int *von2, int *nach, unsigned long *rest,
                    int links, int rechts)
{
    int i, j;
    unsigned long pivot;

    i = links;
    j = rechts - 1;
    pivot = rest[rechts];

    do {
        while (rest[i] <= pivot && i < rechts)
            i++;

        while (rest[j] >= pivot && j > links)
            j--;

        if (i < j) {
            SWAP(rest[i], rest[j], unsigned long);
            SWAP(von[i], von[j], int);
            SWAP(von2[i], von2[j], int);
            SWAP(nach[i], nach[j], int);
        }
    } while (i < j);

    if (rest[i] > pivot) {
        SWAP(rest[i], rest[rechts], unsigned long);
        SWAP(von[i], von[rechts], int);
        SWAP(von2[i], von2[rechts], int);
        SWAP(nach[i], nach[rechts], int);
    }

    return i;
}

/* Beschreibung:
 *     sortiert die 4 Arrays nach dem Restvolumen (aufsteigend)
 * Eingabewerte:
 *     1. bis 4. Argument: die zu sortierenden Arrays
 *                         (das vierte Array enthaelt die Restvolumina)
 *     5. Argument: untere Grenze
 *     6. Argument: obere Grenze
 * Rueckgabewerte:
 *     keine */
static void quicksort(int *von, int *von2, int *nach, unsigned long *rest,
                      int links, int rechts)
{
    int index;

    if (links >= rechts)
        return;

    index = sortiere(von, von2, nach, rest, links, rechts);

    quicksort(von, von2, nach, rest, links, index - 1);
    quicksort(von, von2, nach, rest, index + 1, rechts);
}

/* Beschreibung:
 *     packt die Kisten mit minimalen Zwischenraum
 * Eingabewert:
 *     Zeiger auf die Kellerstruktur
 * Rueckgabewerte:
 *     keine */
static void packe_mit_minimalen_zwischenraum(struct keller *k)
{
    int i, j, l, kombinationen;
    int *von, *von2, *nach;
    unsigned long *rest;

    kombinationen = 0;
    von = von2 = nach = NULL;
    rest = NULL;

    /* jede Kiste mit jeder kombinieren */
    for (i = 0; i < k->anzahl_kisten; i++)
        for (j = 0; j < k->anzahl_kisten; j++) {
            if (i == j)
                continue;

            if (kann_gepackt_werden(k->kiste[i], k->kiste[j])) {
                kombinationen++;

                /* Speicher fuer neue Kombination holen */
                if ((von = realloc(von, sizeof(int)
                                   * kombinationen)) == NULL
                    || (von2 = realloc(von2, sizeof(int)
                                       * kombinationen)) == NULL
                    || (nach = realloc(nach, sizeof(int)
                                       * kombinationen)) == NULL
                    || (rest = realloc(rest, sizeof(unsigned long)
                                       * kombinationen)) == NULL) {
                    perror("realloc");
                    exit(EXIT_FAILURE);
                }

                /* Kombination merken */
                von[kombinationen - 1] = i;
                von2[kombinationen - 1] = -1;
                nach[kombinationen - 1] = j;

                /* Zwischenraum berechnen */
                rest[kombinationen - 1] = k->kiste[j]->volumen -
                    k->kiste[i]->volumen;
            }
        }

    /* Paare bilden */
    for (i = 0; i < k->anzahl_kisten; i++)
        for (j = i + 1; j < k->anzahl_kisten; j++)
            for (l = 0; l < k->anzahl_kisten; l++) {
                if (j == l || i == l)
                    continue;

                if (koennen_gepackt_werden(k->kiste[i], k->kiste[j],
                                           k->kiste[l])) {
                    kombinationen++;

                    /* Speicher fuer neue Kombination holen */
                    if ((von = realloc(von, sizeof(int)
                                       * kombinationen)) == NULL
                        || (von2 = realloc(von2, sizeof(int)
                                           * kombinationen)) == NULL
                        || (nach = realloc(nach, sizeof(int)
                                           * kombinationen)) == NULL
                        || (rest = realloc(rest, sizeof(unsigned long)
                                           * kombinationen)) == NULL) {
                        perror("realloc");
                        exit(EXIT_FAILURE);
                    }

                    /* Kombination merken */
                    von[kombinationen - 1] = i;
                    von2[kombinationen - 1] = j;
                    nach[kombinationen - 1] = l;

                    /* Zwischenraum berechnen */
                    rest[kombinationen - 1] = k->kiste[l]->volumen -
                        k->kiste[i]->volumen - k->kiste[j]->volumen;
                }
            }

    if (kombinationen == 0)
        return;

    /* Sortieren der Arrays nach dem Restvolumen (aufsteigend) */
    quicksort(von, von2, nach, rest, 0, kombinationen - 1);

    for (i = 0; i < kombinationen; i++) {
        if (von2[i] == -1) {
            if (kann_gepackt_werden(k->kiste[von[i]], k->kiste[nach[i]]))
                kiste_packen(k->kiste[von[i]], k->kiste[nach[i]]);
        } else {
            if (koennen_gepackt_werden(k->kiste[von[i]], k->kiste[von2[i]],
                                       k->kiste[nach[i]]))
                kisten_packen(k->kiste[von[i]], k->kiste[von2[i]],
                              k->kiste[nach[i]]);
        }
    }

    free(von);
    free(von2);
    free(nach);
    free(rest);
}

/* Beschreibung:
 *     - sucht nach einer besseren Loesung,
 *       indem die Kisten mit minimalen Zwischenraum gepackt werden
 *     - die Anleitung zum Packen der Kisten wird nur dann ausgegeben,
 *       wenn eine bessere Loesung gefunden wurde
 * Eingabewert:
 *     Zeiger auf die Kellerstruktur
 * Rueckgabewerte:
 *     keine */
void finde_loesung_minimaler_zwischenraum(struct keller *k)
{
    reset_keller(k);

    packe_mit_minimalen_zwischenraum(k);

    berechne_aeusseres_volumen(k);
}
