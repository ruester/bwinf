/* Bundeswettbewerb Informatik
 * Aufgabe 2 - Robuttons
 *
 * Autor: Matthias Rüster
 *
 * E-Mail: ruester@molgen.mpg.de
 */

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "tisch.h"
#include "vektor.h"

/* Beschreibung:
 *     holt Speicher fuer eine Tischstruktur
 * Eingabewerte:
 *     Anzahl der Ecken des Tisches
 * Rueckgabewerte:
 *     Zeiger auf die Tischstruktur */
struct tisch *neuer_tisch(int anzahl_ecken)
{
    struct tisch *t;

    /* Speicher holen */
    if ((t = calloc(1, sizeof(struct tisch))) == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    /* Speicher fuer Koordinaten der Ecken holen */
    t->ecken = neue_vektoren(anzahl_ecken);

    t->anzahl_ecken = anzahl_ecken;

    /* Zeiger zurueckgeben */
    return (t);
}

/* Beschreibung:
 *     holt Speicher fuer eine Tischstruktur (Polygon)
 * Eingabewerte:
 *     1. Argument: Array von Vektoren mit den Koordinaten der Ecken
 *     2. Argument: Anzahl der Ecken des Tisches
 * Rueckgabewerte:
 *     Zeiger auf die Tischstruktur */
struct tisch *neuer_poly_tisch(struct vektor *ecken, int anzahl_ecken)
{
    struct tisch *t;

    /* Speicher holen */
    if ((t = calloc(1, sizeof(struct tisch))) == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    /* Typ festlegen */
    t->typ = POLYGON;

    /* die Tischstruktur bekommt die Koordinaten der Ecken vom 1. Argument */
    t->ecken = ecken;

    t->anzahl_ecken = anzahl_ecken;

    /* zeiger zurueckgeben */
    return (t);
}

/* Beschreibung:
 *     holt Speicher fuer eine Tischstruktur (runder Tisch)
 * Eingabewerte:
 *     1. Argument: Koordinaten des Mittelpunktes des Kreises
 *     2. Argument: Radius des Kreises
 * Rueckgabwerte:
 *     Zeiger auf die Tischstruktur */
struct tisch *neuer_runder_tisch(struct vektor mittelpunkt, double radius)
{
    struct tisch *t;

    /* Speicher fuer einen Tisch mit 2 Ecken holen */
    t = neuer_tisch(2);
    t->typ = KREIS;

    /* Mittelpunkt speichern */
    t->ecken[0].x = mittelpunkt.x;
    t->ecken[0].y = mittelpunkt.y;

    /* Radius speichern */
    t->ecken[1].x = radius;

    /* Zeiger zurueckgeben */
    return (t);
}

/* Beschreibung:
 *     holt Speicher fuer eine Tischstruktur (viereckiger Tisch)
 * Eingabewerte:
 *     1. Argument: Koordinate der oberen linken Ecke
 *     2. Argument: Koordinate der unteren rechten Ecke
 * Rueckgabewerte:
 *     Zeiger auf die Tischstruktur */
struct tisch *neuer_viereckiger_tisch(struct vektor v1, struct vektor v2)
{
    struct tisch *t;

    /* Speicher holen */
    t = neuer_tisch(5);
    t->typ = VIERECK;

    /* Koordinaten der Ecken bestimmen */
    t->ecken[0].x = v1.x;
    t->ecken[0].y = v1.y;

    t->ecken[1].x = v2.x;
    t->ecken[1].y = v1.y;

    t->ecken[2].x = v2.x;
    t->ecken[2].y = v2.y;

    t->ecken[3].x = v1.x;
    t->ecken[3].y = v2.y;

    t->ecken[4].x = v1.x;
    t->ecken[4].y = v1.y;

    /* Zeiger zurueckgeben */
    return (t);
}

/* Beschreibung:
 *     prueft ob eine Koordinate Teil der Tischflaeche ist
 * Eingabewerte:
 *     1. Argument: Zeiger auf die Tischstruktur
 *     2. Argument: x-Wert der Koordinate
 *     3. Argument: y-Wert der Koordinate
 * Rueckgabewerte:
 *     0, wenn die Koordinate nicht zur tischflaeche gehoert
 *     1, wenn die Koordinate zur Tischflaeche gehoert */
char ist_tischflaeche(struct tisch *t, double x, double y)
{
    int i, j,                   /* Zaehlvariablen */
     schnittpunkte;             /* Anzahl der Schnittpunkte */
    struct strecke a,           /* Strecke zwischen zwei Ecken */
     b;                         /* Strahl in x-Richtung */
    struct vektor m, v;         /* bei einem runden Tisch benoetigt */
    struct vektor *s,           /* Array von Schnittpunkten */
    *r;                         /* Schnittpunkt zwischen
                                 * Strecke und Strahl */

    /* Typ des Tisches unterscheiden */
    switch (t->typ) {
    case KREIS:
        /* Koordinate in einem Vektor speichern */
        v.x = x;
        v.y = y;

        /* Mittelpunkt in einem Vektor speichern */
        m.x = t->ecken[0].x;
        m.y = t->ecken[0].y;

        /* pruefen ob der Abstand der Vektoren groesser als der Radius ist */
        if (t->ecken[1].x >= abstand(v, m))
            return (1);         /* Koordinate gehoert zur Tischflaeche */
        break;

    case VIERECK:
        /* pruefen, ob Koordinate im Viereck liegt */
        if (x >= t->ecken[0].x && x <= t->ecken[2].x
            && y >= t->ecken[0].y && y <= t->ecken[2].y)
            return (1);         /* Koordinate gehoert zur Tischflaeche */
        break;

    case POLYGON:
        /* Speicher fuer ein Vektorenarray holen */
        s = neue_vektoren(t->anzahl_ecken * 2);
        schnittpunkte = 0;

        /* Strahl erstellen */
        b.anfang.x = x;
        b.anfang.y = y;
        b.ende.x = x + 1.0;
        b.ende.y = y;

        /* Schnittpunkte des Strahls mit dem Polygon ermitteln */
        for (i = 0; i < t->anzahl_ecken - 1; i++) {
            /* Strecke erstellen */
            a.anfang.x = t->ecken[i].x;
            a.anfang.y = t->ecken[i].y;
            a.ende.x = t->ecken[i + 1].x;
            a.ende.y = t->ecken[i + 1].y;

            /* Sonderfall: Strahl ist "identisch" mit der Strecke */
            if (a.anfang.y == b.anfang.y && a.ende.y == b.anfang.y
                && b.anfang.x <= a.ende.x) {
                /* Schnittpunkt speichern */
                s[schnittpunkte].x = a.ende.x;
                s[schnittpunkte].y = a.ende.y;

                /* Anzahl der Schnittpunkte erhoehen */
                schnittpunkte++;
                continue;
            }

            /* Schnittpunkt mit Strecke ermitteln */
            r = schnittpunkt_strecke_strahl(a, b);

            /* Schnittpunkt vorhanden */
            if (r != NULL) {
                /* Schnittpunkt speichern */
                s[schnittpunkte].x = r->x;
                s[schnittpunkte].y = r->y;

                /* doppelte Schnittpunkte ueberpruefen */
                if (schnittpunkte > 0
                    && s[schnittpunkte].x == s[schnittpunkte - 1].x
                    && s[schnittpunkte].y == s[schnittpunkte - 1].y) {
                    /* der Schnittpunkt ist die Ecke mit dem Index i */
                    if (r->x == a.anfang.x && r->y == a.anfang.y) {
                        /* y-Werte der Ecken ueberpruefen */
                        if ((t->ecken[i].y <= t->ecken[i - 1].y
                             && t->ecken[i].y >= t->ecken[i + 1].y)
                            || (t->ecken[i].y >= t->ecken[i - 1].y
                                && t->ecken[i].y <= t->ecken[i + 1].y))
                            /* beide Schnittpunkte zaehlen
                             * nur als ein Schnittpunkt */
                            schnittpunkte--;
                    } else {
                        /* der Schnittpunkt ist die Ecke mit dem Index i+1 */
                        if (r->x == a.ende.x && r->y == a.ende.y) {
                            if (i == t->anzahl_ecken - 2)
                                j = 1;
                            else
                                j = i + 2;

                            /* y-Werte der Ecken ueberpruefen */
                            if ((t->ecken[i + 1].y <= t->ecken[j].y
                                 && t->ecken[i + 1].y >= t->ecken[i].y)
                                || (t->ecken[i + 1].y >= t->ecken[j].y
                                    && t->ecken[i + 1].y <= t->ecken[i].y))
                                /* beide Schnittpunkte zaehlen
                                 * nur als ein Schnittpunkt */
                                schnittpunkte--;
                        }
                    }
                }

                /* Anzahl der Schnittpunkte erhoehen */
                schnittpunkte++;

                /* Speicher freigeben */
                free(r);
            }
        }

        /* Speicher des Vektorenarrays freigeben */
        free(s);

        /* ungerade Anzahl an Schnittpunkten pruefen */
        if (schnittpunkte % 2)
            return (1);         /* Koordinate gehoert zur Tischflaeche */
    }

    /* Koordinate gehoert nicht zur Tischflaeche */
    return (0);
}

/* Beschreibung:
 *     gibt den Speicher fuer eine Tischstruktur frei
 * Eingabewerte:
 *     Zeiger auf die Tischstruktur
 * Rueckgabewerte:
 *     keine */
void tisch_freigeben(struct tisch *t)
{
    free(t->ecken);
    free(t);
}
