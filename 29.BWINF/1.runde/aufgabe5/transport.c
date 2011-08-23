/* Bundeswettbewerb Informatik
 * Aufgabe 5 - Pyramide
 *
 * Autor: Matthias Rüster
 *
 * E-Mail: ruester@molgen.mpg.de
 */

#include <stdlib.h>
#include <stdio.h>

#include "transport.h"
#include "block.h"

/* Beschreibung:
 *     holt Speicher fuer eine Transportstruktur
 * Eingabewerte:
 *     1. - 3. Argument: Ursprungskoordinate
 *     4. - 6. Argument: Zielkoordinate
 *     7. Argument: Blocktyp des Blocks
 * Rueckgabewerte:
 *     Zeiger auf die erstellte Transportstruktur */
struct transport *neuer_transport(int k, int x, int y,
                                  int k_ziel, int x_ziel, int y_ziel,
                                  char typ)
{
    struct transport *t;

    /* Speicher holen */
    if ((t = calloc(1, sizeof(struct transport))) == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    /* Werte zuweisen */
    t->k = k;
    t->x = x;
    t->y = y;
    t->k_ziel = k_ziel;
    t->x_ziel = x_ziel;
    t->y_ziel = y_ziel;
    t->blocktyp = typ;
    t->next = NULL;

    /* Zeiger zurueckgeben */
    return (t);
}

/* Beschreibung:
 *     holt Speicher fuer eine Transportliste
 * Eingabewerte:
 *     keine
 * RueckgabewerteL
 *     Zeiger auf die Transportliste */
struct transportliste *neue_transportliste()
{
    struct transportliste *l;
    
    if((l = calloc(1, sizeof(struct transportliste))) == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }
    
    return(l);
}

/* Beschreibung:
 *     fuegt eine Transportstruktur den Pyramidenumzug hinzu
 * Eingabewerte:
 *     1. Argument: Zeiger auf den Pyramidenumzug
 *     2. Argument: Zeiger auf die Transportstruktur
 * Rueckgabewerte:
 *     keine */
void transport_hinzufuegen(struct transportliste *l,
                           struct transport *t)
{
    struct transport *curr;

    if (l->first == NULL) {
        l->first = t;
        l->anzahl_transporte++;
        return;
    }

    curr = l->first;

    while (curr->next != NULL)
        curr = curr->next;

    curr->next = t;
    l->anzahl_transporte++;
}


/* Beschreibung:
 *     gibt die Daten einer Transportstruktur aus
 * Eingabewerte:
 *     Zeiger auf die transportstruktur
 * Rueckgabewerte:
 *     keine */
void print_transport(struct transport *t)
{
    switch (t->blocktyp) {
    case MITTELSTEIN:
        printf("Mittelstein ");
        break;

    case KANTENSTEIN:
        printf("Kantenstein ");
        break;

    case ECKSTEIN:
        printf("Eckstein    ");
        break;

    case SPITZE:
        printf("Spitze      ");
        break;
    }

    if (t->k == LAGER || t->x == LAGER || t->y == LAGER)
        printf("vom Lager %-29c nach Luxor (k = %-4d x = %-4d y = %-4d)",
               ' ', t->k_ziel + 1, t->x_ziel + 1, t->y_ziel + 1);
    else if (t->k_ziel == LAGER || t->x_ziel == LAGER || t->y_ziel == LAGER)
        printf("von Theben (k = %-4d x = %-4d y = %-4d) zum Lager",
               t->k + 1, t->x + 1, t->y + 1);
    else {
        printf("von Theben (k = %-4d x = %-4d y = %-4d) ",
               t->k + 1, t->x + 1, t->y + 1);
        printf("nach Luxor (k = %-4d x = %-4d y = %-4d)",
               t->k_ziel + 1, t->x_ziel + 1, t->y_ziel + 1);
    }

    printf("\n");
}

/* Beschreibung:
 *     gibt die Anleitung zum Umzug der Pyramide aus
 * Eingabewerte:
 *     Zeiger auf den Pyramidenumzug
 * Rueckgabewerte:
 *     keine */
void print_transportliste(struct transportliste *l)
{
    struct transport *curr;

    printf("Transport:\n");

    curr = l->first;

    /* Transportliste durchlaufen */
    while (curr != NULL) {
        /* Transport ausgeben */
        print_transport(curr);
        curr = curr->next;
    }

    /* Anzahl der Transporte ausgeben */
    printf("Anzahl Transporte: %d\n", l->anzahl_transporte);
}

/* Beschreibung:
 *     gibt den Speicher einer Transportliste frei
 * Eingabewerte:
 *     Zeiger auf die Transportliste
 * Rueckgabewerte:
 *     keine */
void transportliste_freigeben(struct transportliste *l)
{
    struct transport *t;

    t = l->first;

    /* Transportliste durchlaufen */
    while (t != NULL) {
        l->first = t;
        t = t->next;
        
        /* Speicher des Listenelements freigeben */
        free(l->first);
    }
    
    free(l);
}
