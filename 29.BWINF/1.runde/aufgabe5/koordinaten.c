/* Bundeswettbewerb Informatik
 * Aufgabe 5 - Pyramide
 *
 * Autor: Matthias Rüster
 *
 * E-Mail: ruester@molgen.mpg.de
 */

#include <stdio.h>
#include <stdlib.h>

#include "koordinaten.h"

/* Beschreibung:
 *     erstellt eine Koordinatenliste
 * Eingabewerte:
 *     keine
 * Rueckgabewerte:
 *     Zeiger auf die erstellte Koordinatenliste */
struct koordinatenliste *neue_koordinatenliste()
{
    struct koordinatenliste *l;

    /* Speicher holen */
    if ((l = calloc(1, sizeof(struct koordinatenliste))) == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    /* Zeiger zurueckgeben */
    return (l);
}

/* Beschreibung:
 *     erstellt eine Koordinate
 * Eingabewerte:
 *     1. Argument: Schicht
 *     2. Arugment: x-Wert
 *     3. Argument: y-Wert
 * Rueckgabewerte:
 *     Zeiger auf die erstellte Koordinate */
struct koordinate *neue_koordinate(int k, int x, int y)
{
    struct koordinate *ko;

    /* Speicher holen */
    if ((ko = calloc(1, sizeof(struct koordinate))) == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    /* Werte zuweisen */
    ko->k = k;
    ko->x = x;
    ko->y = y;

    /* Zeiger zurueckgeben */
    return (ko);
}

/* Beschreibung:
 *     gibt den Speicher fuer eine Koordinatenliste frei
 * Eingabewerte:
 *     Zeiger auf die Koordinatenliste
 * Rueckgabewerte:
 *     keine */
void loesche_koordinatenliste(struct koordinatenliste *l)
{
    struct koordinate *curr, *temp;

    curr = l->first;

    while (curr != NULL) {
        temp = curr;
        curr = curr->next;
        free(temp);
    }

    free(l);
}

/* Beschreibung:
 *     fuegt eine Koordinate einer Koordinatenliste hinzu
 * Eingabewerte:
 *     1. Argument: Zeiger auf die Koordinatenliste
 *     2. Argument: Zeiger auf die Koordinate
 * Rueckgabewerte:
 *     keine */
void koordinate_hinzufuegen(struct koordinatenliste *l, struct koordinate *k)
{
    if (l->first == NULL) {
        l->first = k;
        l->last = k;
        return;
    }

    l->last->next = k;
    l->last = k;
}

/* Beschreibung:
 *     - loescht eine Koordinate aus einer Koordinatenliste
 *     - gibt den Speicher der Koordinate frei
 * Eingabewerte:
 *     1. Argument: Zeiger auf die Koordinatenliste
 *     2. Argument: Zeiger auf die Koordinate
 * Rueckgabewerte:
 *     keine */
void loesche_koordinate(struct koordinatenliste *kl, struct koordinate *k)
{
    struct koordinate *temp;

    if (k == kl->first) {
        kl->first = k->next;
        free(k);
        return;
    }

    temp = kl->first;

    while (temp->next != k)
        temp = temp->next;

    temp->next = k->next;

    if (k == kl->last)
        kl->last = temp;

    free(k);
}
