/* 29. Bundeswettbewerb Informatik - 2. Runde
 * Aufgabe 1 - Kisten in Kisten in Kisten
 *
 * Autor: Matthias Rüster
 *
 * E-Mail: ruester@molgen.mpg.de
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "keller.h"

static void hilfe()
{
    printf("Aufruf des Programms: ./kisteninkisten [Optionen] <Dateiname>\n");
    printf("moegliche Optionen:\n");
    printf("\t-k [Kompression]\tmaximale Kompression festlegen\n");
}

static char pruefe_kompression(struct keller *k, double max_kompression)
{
    double kompression;

    kompression = (double) k->gesamt_volumen / (double) k->aeusseres_volumen;

    if (kompression >= max_kompression && max_kompression > 1.0) {
        printf("Verschachtelung:\n================\n");
        return 1;
    }

    return 0;
}

double parameter_einlesen(int argc, char *argv[])
{
    int c;
    double max_kompression;

    max_kompression = -1.0;

    while ((c = getopt(argc, argv, "k:")) != -1) {
        switch (c) {
        case 'k':
            if (argc != 4) {
                printf("Der Dateiname wurde nicht angegeben\n");
                hilfe();
                exit(EXIT_FAILURE);
            }

            if ((max_kompression = atof(optarg)) <= 1.0) {
                printf("Falsche Kompression angegeben\n");
                hilfe();
                exit(EXIT_FAILURE);
            }
            break;

        case '?':
            if (optopt == 'k')
                printf("Die Option -%c benoetigt ein Argument\n", optopt);
            else
                printf("unbekannte Option %c\n", optopt);

            hilfe();
            exit(EXIT_FAILURE);
            break;
        }
    }

    return max_kompression;
}

void print_methode(char methode)
{
    printf("Beste Verschachtelung gefunden mit Methode ");

    switch (methode) {
    case 'A':
        printf("A (nur einzeln verpacken):\n");
        break;

    case 'B':
        printf("B (zuerst zwei Kisten verpacken):\n");
        break;

    case 'C':
        printf("C (Quick-Brute-Force):\n");
        break;

    case 'D':
        printf("D (minimaler Zwischenraum):\n");
        break;
    }
}

int main(int argc, char *argv[])
{
    struct keller *k, *l1, *l2, *l3, *l4;
    double max_kompression;
    unsigned long min;
    char temp;

    /* Parameter ueberpruefen */
    if (argc < 2) {
        hilfe();
        return 1;
    }

    max_kompression = parameter_einlesen(argc, argv);

    l1 = l2 = l3 = l4 = NULL;

    /* Kisten einlesen */
    k = neuer_keller_von_datei(argv[argc - 1]);

    if (k->anzahl_kisten <= 1) {
        if (k->anzahl_kisten == 0)
            printf("Der Keller enthaelt keine Kisten\n");
        else
            printf("Im Keller ist nur eine Kiste\n");
        keller_loeschen(k);
        return 0;
    }

    /* eingelesene Kisten ausgeben */
    print_kisten(k);

    /* Methode A und B ausfuehren */
    temp = finde_loesung_schnell(k);

    /* Ergebnis auswerten */
    if (pruefe_kompression(k, max_kompression)) {
        if (temp)
            print_methode('A');
        else
            print_methode('B');

        print_anleitung(k);
        keller_loeschen(k);
        return 0;
    }

    /* Ergebnis speichern */
    temp ? (l1 = kopie_von_keller(k)) : (l2 = kopie_von_keller(k));

    /* Methode C ausfuehren */
    finde_loesung_quickbruteforce(k);

    /* Ergebnis auswerten */
    if (pruefe_kompression(k, max_kompression)) {
        print_methode('C');
        print_anleitung(k);
        keller_loeschen(k);
        temp ? keller_loeschen(l1) : keller_loeschen(l2);
        return 0;
    }

    /* Ergebnis speichern */
    l3 = kopie_von_keller(k);

    /* Methode D ausfuehren */
    finde_loesung_minimaler_zwischenraum(k);

    /* Ergebnis auswerten */
    if (pruefe_kompression(k, max_kompression)) {
        print_methode('D');
        print_anleitung(k);
        keller_loeschen(k);
        temp ? keller_loeschen(l1) : keller_loeschen(l2);
        keller_loeschen(l3);
        return 0;
    }

    /* Ergebnis speichern */
    l4 = kopie_von_keller(k);

    if (max_kompression != -1)
        printf("Kompression konnte nicht erreicht werden\n\n");

    /* beste Verschachtelung herausfinden */
    min = temp ? l1->aeusseres_volumen : l2->aeusseres_volumen;

    if (min > l3->aeusseres_volumen) {
        min = l3->aeusseres_volumen;

        if (min > l4->aeusseres_volumen) {
            print_methode('D');
            keller_kopieren(l4, k);
            goto finish;
        }

        print_methode('C');
        keller_kopieren(l3, k);
        min = l3->aeusseres_volumen;
        goto finish;
    }

    if (min > l4->aeusseres_volumen) {
        print_methode('D');
        keller_kopieren(l4, k);
        goto finish;
    }

    if (temp) {
        print_methode('A');
        keller_kopieren(l1, k);
    } else {
        print_methode('B');
        keller_kopieren(l2, k);
    }

  finish:
    print_anleitung(k);

    /* Speicher freigeben */
    temp ? keller_loeschen(l1) : keller_loeschen(l2);
    keller_loeschen(l3);
    keller_loeschen(l4);
    keller_loeschen(k);

    return 0;
}
