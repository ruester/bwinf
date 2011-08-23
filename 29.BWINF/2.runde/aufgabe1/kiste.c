/* 29. Bundeswettbewerb Informatik - 2. Runde
 * Aufgabe 1 - Kisten in Kisten in Kisten
 *
 * Autor: Matthias Rüster
 *
 * E-Mail: ruester@molgen.mpg.de
 */

#include <stdlib.h>
#include <stdio.h>

#include "kiste.h"

#define MAX(a, b) ((a > b) ? (a) : (b))

/* Beschreibung:
 *     berechnet das Volumen von einer Kiste
 * Eingabewert:
 *     Zeiger auf die Kistenstruktur
 * Rueckgabewert:
 *     das Volumen der Kiste */
static unsigned long volumen(struct kiste *k)
{
    return k->l * k->b * k->h;
}

/* Beschreibung:
 *     erstellt eine Kistenstruktur und initialisiert diese
 * Eingabewerte:
 *     1. Argument: Laenge der Kiste
 *     2. Argument: Breite der Kiste
 *     3. Argument: Hoehe der Kiste
 *     4. Argument: Index der Kiste
 * Rueckgabewert:
 *     Zeiger auf die erstellte Kistenstruktur */
struct kiste *neue_kiste(int l, int b, int h, int index)
{
    struct kiste *k;

    if ((k = calloc(1, sizeof(struct kiste))) == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    k->l = l;
    k->b = b;
    k->h = h;
    k->verpackt = 0;
    k->index = index;
    k->volumen = volumen(k);

    return k;
}

/* Beschreibung:
 *     kopiert die Daten einer Kistenstruktur
 * Eingabewerte:
 *     1. Argument: Zeiger auf die Kistenstruktur, die kopiert werden soll
 *     2. Argument: Zeiger auf die Kistenstruktur (Ziel des Kopiervorgangs)
 * Rueckgabewerte:
 *     keine */
void kiste_kopieren(struct kiste *von, struct kiste *nach)
{
    nach->l = von->l;
    nach->b = von->b;
    nach->h = von->h;
    nach->index = von->index;
    nach->verpackt = von->verpackt;
    nach->volumen = von->volumen;
}

/* Beschreibung:
 *     prueft ob eine Kiste in eine andere passt
 * Eingabewerte:
 *     1. Argument: Zeiger auf die Kistenstruktur, die verpackt werden soll
 *     2. Argument: Zeiger auf die Kistenstruktur,
 *                  in der die Kiste gepackt werden soll
 * Rueckgabewerte:
 *     1, wenn Kiste 'von' in Kiste 'nach' gepackt werden kann
 *     0, wenn Kiste 'von' nicht in Kiste 'nach' gepackt werden kann */
char kann_gepackt_werden(struct kiste *von, struct kiste *nach)
{
    /* Kiste 'nach' ist schon voll */
    if (nach->inhalt[0] != NULL || nach->inhalt[1] != NULL)
        return 0;

    /* Kiste 'von' ist schon verpackt */
    if (von->verpackt)
        return 0;

    /* Kiste 'von' ist groesser als Kiste 'nach' */
    if (von->volumen >= nach->volumen)
        return 0;

    /* keine Drehung */
    if (von->l < nach->l && von->b < nach->b && von->h < nach->h)
        return 1;

    /* 90 Grad Drehung um x-Achse */
    if (von->l < nach->l && von->b < nach->h && von->h < nach->b)
        return 1;

    /* 90 Grad Drehung um y-Achse */
    if (von->l < nach->h && von->b < nach->b && von->h < nach->l)
        return 1;

    /* 90 Grad Drehung um z-Achse */
    if (von->l < nach->b && von->b < nach->l && von->h < nach->h)
        return 1;

    /* 90 Grad Drehung um x- und z-Achse */
    if (von->l < nach->h && von->b < nach->l && von->h < nach->b)
        return 1;

    /* 90 Grad Drehung um x- und y-Achse */
    if (von->l < nach->b && von->b < nach->h && von->h < nach->l)
        return 1;

    return 0;
}

/* Beschreibung:
 *     packt eine Kiste in eine andere
 * Eingabewerte:
 *     1. Argument: Zeiger auf die Kistenstruktur, die verpackt werden soll
 *     2. Argument: Zeiger auf die Kistenstruktur,
 *                  in der die Kiste verpackt wird
 * Rueckgabewerte:
 *     keine */
void kiste_packen(struct kiste *von, struct kiste *nach)
{
    nach->inhalt[0] = von;
    von->verpackt = 1;
}

/* Beschreibung:
 *     prueft ob zwei Kisten in eine andere passen (hintereinander)
 * Eingabewerte:
 *     1. und 2. Argument: Zeiger auf die Kistenstrukturen,
 *                         die verpackt werden sollen
 *     3. Argument: Zeiger auf die Kistenstruktur,
 *                  in der die Kisten verpackt werden
 * Rueckgabewerte:
 *     1, wenn die Kisten in die Kiste gepackt werden koennen
 *     0, wenn die Kisten nicht in die Kiste gepackt werden koennen */
char hintereinander(struct kiste *von, struct kiste *von2,
                    struct kiste *nach)
{
    struct kiste k;
    int i;

    k.verpackt = 0;

    /* zur Verbesserung der Laufzeit: ueberspringe Volumenberechnung */
    k.volumen = 0L;

    for (i = 0; i < 6; i++) {
        switch (i) {
        case 0:
            k.l = von->l + von2->l;
            k.b = MAX(von->b, von2->h);
            k.h = MAX(von->h, von2->b);
            break;
        case 1:

            k.b = MAX(von->b, von2->b);
            k.h = MAX(von->h, von2->h);
            break;
        case 2:
            k.l = von->l + von2->b;
            k.b = MAX(von->b, von2->l);
            k.h = MAX(von->h, von2->h);
            break;
        case 3:

            k.b = MAX(von->b, von2->h);
            k.h = MAX(von->h, von2->l);
            break;
        case 4:
            k.l = von->l + von2->h;
            k.b = MAX(von->b, von2->b);
            k.h = MAX(von->h, von2->l);
            break;
        case 5:

            k.b = MAX(von->b, von2->l);
            k.h = MAX(von->h, von2->b);
            break;
        }

        if (kann_gepackt_werden(&k, nach))
            return 1;
    }

    return 0;
}

/* Beschreibung:
 *     prueft ob zwei Kisten in eine andere passen (nebeneinander)
 * Eingabewerte:
 *     1. und 2. Argument: Zeiger auf die Kistenstrukturen,
 *                         die verpackt werden sollen
 *     3. Argument: Zeiger auf die Kistenstruktur,
 *                  in der die Kisten verpackt werden
 * Rueckgabewerte:
 *     1, wenn die Kisten in die Kiste gepackt werden koennen
 *     0, wenn die Kisten nicht in die Kiste gepackt werden koennen */
char nebeneinander(struct kiste *von, struct kiste *von2,
                   struct kiste *nach)
{
    struct kiste k;
    int i;

    k.verpackt = 0;

    /* zur Verbesserung der Laufzeit: ueberspringe Volumenberechnung */
    k.volumen = 0L;

    for (i = 0; i < 6; i++) {
        switch (i) {
        case 0:
            k.l = MAX(von->l, von2->b);
            k.b = von->b + von2->l;
            k.h = MAX(von->h, von2->h);
            break;
        case 1:
            k.l = MAX(von->l, von2->h);

            k.h = MAX(von->h, von2->b);
            break;
        case 2:
            k.l = MAX(von->l, von2->l);
            k.b = von->b + von2->b;
            k.h = MAX(von->h, von2->h);
            break;
        case 3:
            k.l = MAX(von->l, von2->h);

            k.h = MAX(von->h, von2->l);
            break;
        case 4:
            k.l = MAX(von->l, von2->l);
            k.b = von->b + von2->h;
            k.h = MAX(von->h, von2->b);
            break;
        case 5:
            k.l = MAX(von->l, von2->b);

            k.h = MAX(von->h, von2->l);
            break;
        }

        if (kann_gepackt_werden(&k, nach))
            return 1;
    }

    return 0;
}

/* Beschreibung:
 *     prueft ob zwei Kisten in eine andere passen (uebereinander)
 * Eingabewerte:
 *     1. und 2. Argument: Zeiger auf die Kistenstrukturen,
 *                         die verpackt werden sollen
 *     3. Argument: Zeiger auf die Kistenstruktur,
 *                  in der die Kisten verpackt werden
 * Rueckgabewerte:
 *     1, wenn die Kisten in die Kiste gepackt werden koennen
 *     0, wenn die Kisten nicht in die Kiste gepackt werden koennen */
char uebereinander(struct kiste *von, struct kiste *von2,
                   struct kiste *nach)
{
    struct kiste k;
    int i;

    k.verpackt = 0;

    /* zur Verbesserung der Laufzeit: ueberspringe Volumenberechnung */
    k.volumen = 0L;

    for (i = 0; i < 6; i++) {
        switch (i) {
        case 0:
            k.l = MAX(von->l, von2->h);
            k.b = MAX(von->b, von2->b);
            k.h = von->h + von2->l;
            break;
        case 1:
            k.l = MAX(von->l, von2->b);
            k.b = MAX(von->b, von2->h);

            break;
        case 2:
            k.l = MAX(von->l, von2->l);
            k.b = MAX(von->b, von2->h);
            k.h = von->h + von2->b;
            break;
        case 3:
            k.l = MAX(von->l, von2->h);
            k.b = MAX(von->b, von2->l);

            break;
        case 4:
            k.l = MAX(von->l, von2->l);
            k.b = MAX(von->b, von2->b);
            k.h = von->h + von2->h;
            break;
        case 5:
            k.l = MAX(von->l, von2->b);
            k.b = MAX(von->b, von2->l);

            break;
        }

        if (kann_gepackt_werden(&k, nach))
            return 1;
    }

    return 0;
}

/* Beschreibung:
 *     prueft, ob zwei Kisten in eine andere gepackt werden koennen
 * Eingabewerte:
 *     1. und 2. Argument: Zeiger auf die Kistenstrukturen,
 *                         die verpackt werden sollen
 *     3. Argument: Zeiger auf die Kistenstruktur,
 *                  in der die Kisten verpackt werden
 * Rueckgabewerte:
 *     1, wenn die Kisten in die Kiste gepackt werden koennen
 *     0, wenn die Kisten nicht in die Kiste gepackt werden koennen */
char koennen_gepackt_werden(struct kiste *von, struct kiste *von2,
                            struct kiste *nach)
{
    if (!kann_gepackt_werden(von, nach)
        || !kann_gepackt_werden(von2, nach))
        return 0;

    return hintereinander(von, von2, nach)
        || nebeneinander(von, von2, nach)
        || uebereinander(von, von2, nach);
}

/* Beschreibung:
 *     verpackt zwei Kisten in eine andere
 * Eingabewerte:
 *     1. und 2. Argument: Zeiger auf die Kistenstrukturen,
 *                         die verpackt werden sollen
 *     3. Argument: Zeiger auf die Kistenstruktur,
 *                  in der die Kisten verpackt werden
 * Rueckgabewerte:
 *     keine */
void kisten_packen(struct kiste *von, struct kiste *von2,
                   struct kiste *nach)
{
    nach->inhalt[0] = von;
    nach->inhalt[1] = von2;

    von->verpackt = 1;
    von2->verpackt = 1;
}

/* Beschreibung:
 *     packt eine Kiste aus einer anderen Kiste heraus
 * Eingabewerte:
 *     1. Argument: Zeiger auf die Kistenstruktur, die ausgepackt werden soll
 *     2. Argument: Zeiger auf die Kistenstruktur,
 *                  aus der die Kiste ausgepackt wird
 * Rueckgabewerte:
 *     keine */
void kiste_auspacken(struct kiste *a, struct kiste *b)
{
    b->inhalt[0] = NULL;
    a->verpackt = 0;
}

/* Beschreibung:
 *     packt zwei Kisten aus einer anderen Kiste aus
 * Eingabewerte:
 *     1. und 2. Argument: Zeiger auf die Kistenstrukturen,
 *                         die ausgepackt werden sollen
 *     3. Argument: Zeiger auf die Kistenstruktur,
 *                  aus der die Kisten ausgepackt werden
 * Rueckgabewerte:
 *     keine */
void kisten_auspacken(struct kiste *a, struct kiste *b, struct kiste *c)
{
    c->inhalt[0] = NULL;
    c->inhalt[1] = NULL;
    a->verpackt = 0;
    b->verpackt = 0;
}
