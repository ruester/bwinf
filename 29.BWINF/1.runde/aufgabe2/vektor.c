/* Bundeswettbewerb Informatik
 * Aufgabe 2 - Robuttons
 *
 * Autor: Matthias Rüster
 *
 * E-Mail: ruester@molgen.mpg.de
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "vektor.h"

/* Beziehung zwischen zwei Geraden */
#define PARALLEL     0
#define SCHNITTPUNKT 1

/* Vorzeichen */
#define POSITIV 1
#define NEGATIV 0

/* Beschreibung:
 *     holt Speicher fuer ein Array von Vektoren
 * Eingabewerte:
 *     Anzahl der Vektoren
 * Rueckgabewerte:
 *     Zeiger auf das Array von Vektoren */
struct vektor *neue_vektoren(int anzahl)
{
    struct vektor *v;

    /* Speicher holen */
    if ((v = calloc(anzahl, sizeof(struct vektor))) == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    /* Zeiger auf das Array zurueckgeben */
    return (v);
}

/* Beschreibung:
 *     gibt das Vorzeichen einer Zahl zurueck
 * Eingabewerte:
 *     eine Zahl
 * Rueckgabewerte:
 *     POSITIV, wenn die Zahl positiv ist
 *     NEGATIV, wenn die Zahl negativ ist */
char vorzeichen(double a)
{
    if (a < 0.0)
        return (NEGATIV);

    return (POSITIV);
}

/* Beschreibung:
 *     vergleicht die Vorzeichen der x- und y-Koordinate zweier Vektoren
 * Eingabewerte:
 *     zwei Vektoren
 * Rueckgabewerte:
 *     1, wenn die Vorzeichen der Vektoren gleich sind
 *     0, wenn die Vorzeichen der Vektoren nicht gleich sind */
char vergleiche_vorzeichen(struct vektor a, struct vektor b)
{
    /* Vorzeichen der x- und y-Werte miteinander vergleichen */
    return (vorzeichen(a.x) == vorzeichen(b.x)
            && vorzeichen(a.y) == vorzeichen(b.y));
}

/* Beschreibung:
 *     prueft die lineare Abhaengigkeit zweier Vektoren
 * Eingabewerte:
 *     zwei Vektoren
 * Rueckgabewerte:
 *     1, wenn die Vektoren linear abhaengig sind
 *     0, wenn die Vektoren nicht linear abhaengig sind */
char linear_abhaengig(struct vektor a, struct vektor b)
{
    double r, s;

    /* vertikaler Vektor */
    if (b.x == 0.0)
        return (a.x == 0.0);

    /* horizontaler Vektor */
    if (b.y == 0.0)
        return (a.y == 0.0);

    /* Verhaeltnisse berechnen */
    r = a.x / b.x;
    s = a.y / b.y;

    /* Verhaeltnisse auf Gleichheit pruefen */
    return (r == s);
}

/* Beschreibung:
 *     untersucht die Beziehung zweier Geraden
 * Eingabewerte:
 *     zwei Geraden
 * Rueckgabewerte:
 *     PARALLEL, wenn die Geraden parallel zueinander sind
 *     SCHNITTPUNKT, wenn sich die Geraden schneiden */
char beziehung(struct strecke a, struct strecke b)
{
    struct vektor u, v;

    /* Richtungsvektoren bestimmen */
    u.x = a.ende.x - a.anfang.x;
    u.y = a.ende.y - a.anfang.y;

    v.x = b.ende.x - b.anfang.x;
    v.y = b.ende.y - b.anfang.y;

    /* lineare Abhaengigkeit pruefen */
    if (linear_abhaengig(u, v))
        return (PARALLEL);

    return (SCHNITTPUNKT);
}

/* Beschreibung:
 *     berechnet den Schnittpunkt zwischen einer Strecke und einem Kreis
 * Eingabewerte:
 *     1. Argument: eine Strecke
 *     2. Argument: Ortvektor des Mittelpunktes des Kreises
 *     3. Argument: Radius des Kreises
 * Rueckgabewerte:
 *     Zeiger auf den Ortsvektor des Schnittpunktes
 *     NULL, wenn kein Schnittpunkt vorhanden ist */
struct vektor *schnittpunkt_strecke_kreis(struct strecke a,
                                          struct vektor mittelpunkt,
                                          double r)
{
    double delta_x, m, n, p, q, t, h;
    struct vektor s1, s2, k1, k2;
    struct vektor *schnittpunkt;

    /* Speicher holen */
    schnittpunkt = neue_vektoren(1);

    delta_x = (a.ende.x - a.anfang.x);

    /* vertikaler Richtungsvektor der Strecke */
    if (delta_x == 0.0) {
        /* Schnittpunkte berechnen */
        s1.x = a.anfang.x;
        s1.y = sqrt(r * r - pow(s1.x - mittelpunkt.x, 2.0)) + mittelpunkt.y;

        s2.x = a.anfang.x;
        s2.y = -sqrt(r * r - pow(s2.x - mittelpunkt.x, 2.0)) + mittelpunkt.y;
    } else {
        /* Geradengleichung der Strecke bestimmen */
        m = (a.ende.y - a.anfang.y) / delta_x;
        n = a.anfang.y - m * a.anfang.x;

        /* Schnittpunkte berechnen (p-q-Formel) */
        p = (2.0 * (m * n - m * mittelpunkt.y - mittelpunkt.x))
            / (m * m + 1.0);
        q = (mittelpunkt.x * mittelpunkt.x +
             mittelpunkt.y * mittelpunkt.y + n * n - r * r -
             2.0 * n * mittelpunkt.y) / (m * m + 1.0);

        t = p / 2.0;
        h = sqrt(t * t - q);

        s1.x = -t + h;
        s1.y = s1.x * m + n;

        s2.x = -t - h;
        s2.y = s2.x * m + n;
    }

    /* Richtungsvektor der Strecke bestimmen */
    k2.x = a.ende.x - a.anfang.x;
    k2.y = a.ende.y - a.anfang.y;

    /* Richtungsvektor vom Streckenanfang
       zum ersten Schnittpunkt bestimmen */
    k1.x = s1.x - a.anfang.x;
    k1.y = s1.y - a.anfang.y;

    /* Vorzeichen der beiden Richtungsvektoren vergleichen */
    if (vergleiche_vorzeichen(k1, k2)
        || (k1.x == 0.0 && k1.y == 0.0)) {
        /* der erste Schnittpunkt ist der Richtige */
        schnittpunkt->x = s1.x;
        schnittpunkt->y = s1.y;

        return (schnittpunkt);
    }

    /* Richtungsvektor vom Streckenanfang
       zum zweiten Schnittpunkt bestimmen */
    k1.x = s2.x - a.anfang.x;
    k1.y = s2.y - a.anfang.y;

    /* Vorzeichen der beiden Richtungsvektoren vergleichen */
    if (vergleiche_vorzeichen(k1, k2)
        || (k1.x == 0.0 && k1.y == 0.0)) {
        /* der zweite Schnittpunkt ist der Richtige */
        schnittpunkt->x = s2.x;
        schnittpunkt->y = s2.y;

        return (schnittpunkt);
    }

    /* Speicher freigeben */
    free(schnittpunkt);

    /* kein Schnittpunkt zwischen der Strecke und dem Kreis */
    return (NULL);
}

/* Beschreibung:
 *     berechnet den Schnittpunkt zwischen zwei Strecken
 * Eingabewerte:
 *     zwei Strecken
 * Rueckgabewerte:
 *     Zeiger auf den Ortsvektor des Schnittpunktes
 *     NULL, wenn kein Schnittpunkt vorhanden ist */
struct vektor *schnittpunkt_strecken(struct strecke a, struct strecke b)
{
    struct vektor *p;
    struct vektor u, v;
    double r, s;
    char rel;
    double A, B, C, D, E;

    /* Beziehung zwischen den beiden Strecken bestimmen */
    rel = beziehung(a, b);

    if (rel == PARALLEL)
        return (NULL);          /* kein Schnittpunkt */

    /* Speicher holen */
    p = neue_vektoren(1);

    /* Richtungsvektoren bestimmen */
    u.x = a.ende.x - a.anfang.x;
    u.y = a.ende.y - a.anfang.y;

    v.x = b.ende.x - b.anfang.x;
    v.y = b.ende.y - b.anfang.y;

    /* horizontale Strecke */
    if (u.x == 0) {
        /* Schnittpunkt berechnen */
        s = (a.anfang.x - b.anfang.x) / v.x;
        p->x = b.anfang.x + s * v.x;
        p->y = b.anfang.y + s * v.y;

        /* pruefen, ob Schnittpunkt auf der Strecke liegt */
        if (s >= 0.0 && s <= 1.0)
            return (p);         /* Schnittpunkt zurueckgeben */

        /* Speicher freigeben */
        free(p);

        /* kein Schnittpunkt zwischen den Strecken */
        return (NULL);
    }

    /* Schnittpunkt berechnen */
    A = a.anfang.y - b.anfang.y;
    B = u.y * (b.anfang.x - a.anfang.x);
    C = u.y * v.x;
    D = u.x;
    E = v.y;

    s = (A + B / D) / (E - C / D);

    A = b.anfang.x - a.anfang.x;
    B = v.x * s;

    r = (A + B) / D;

    p->x = a.anfang.x + r * u.x;
    p->y = a.anfang.y + r * u.y;

    /* pruefen, ob Schnittpunkt auf den Strecken liegt */
    if (r >= 0.0 && r <= 1.0 && s >= 0.0 && s <= 1.0)
        return (p);             /* Schnittpunkt zurueckgeben */

    /* Speicher freigeben */
    free(p);

    /* kein Schnittpunkt zwischen den Strecken */
    return (NULL);
}

/* Beschreibung:
 *     berechnet den Schnittpunkt zwischen einer Strecke und einem Strahl
 * Eingabewerte:
 *     1. Argument: eine Strecke
 *     2. Argument: ein Strahl
 * Ausgabewerte:
 *     Zeiger auf den Ortsvektor des Schnittpunktes
 *     NULL, wenn keine Schnittpunkt vorhanden ist */
struct vektor *schnittpunkt_strecke_strahl(struct strecke a,
                                           struct strecke b)
{
    struct vektor *p;
    struct vektor u, v;
    double r, s;
    double A, B, C, D, E;
    char rel;

    /* Beziehung zwischen der Strecke und dem Strahl bestimmen */
    rel = beziehung(a, b);

    if (rel == PARALLEL)
        return (NULL);          /* kein Schnittpunkt */

    /* Speicher holen */
    p = neue_vektoren(1);

    /* Richtungsvektoren bestimmen */
    u.x = a.ende.x - a.anfang.x;
    u.y = a.ende.y - a.anfang.y;

    v.x = b.ende.x - b.anfang.x;
    v.y = b.ende.y - b.anfang.y;

    /* horizontale Strecke */
    if (u.x == 0) {
        /* Schnittpunkt berechnen */
        s = (a.anfang.x - b.anfang.x) / v.x;
        p->x = b.anfang.x + s * v.x;
        p->y = b.anfang.y + s * v.y;

        /* pruefen, ob Schnittpunkt auf der Strecke liegt */
        if (s >= 0 && ((p->y >= a.anfang.y && p->y <= (a.anfang.y + u.y))
                       || (p->y <= a.anfang.y
                           && p->y >= (a.anfang.y + u.y))))
            return (p);         /* Schnittpunkt zurueckgeben */

        /* Speicher freigeben */
        free(p);

        /* kein Schnittpunkt zwischen der Strecke und dem Strahl */
        return (NULL);
    }

    /* Schnittpunkt berechnen */
    A = a.anfang.y - b.anfang.y;
    B = u.y * (b.anfang.x - a.anfang.x);
    C = u.y * v.x;
    D = u.x;
    E = v.y;

    s = (A + B / D) / (E - C / D);

    A = b.anfang.x - a.anfang.x;
    B = v.x * s;
    C = u.x;

    r = (A + B) / C;

    p->x = a.anfang.x + r * u.x;
    p->y = a.anfang.y + r * u.y;

    /* pruefen, ob Schnittpunkt auf der Strecke liegt */
    if (r >= 0.0 && r <= 1.0 && s >= 0.0)
        return (p);

    /* Speicher freigeben */
    free(p);

    /* kein Schnittpunkt zwischen der Strecke und dem Strahl */
    return (NULL);
}

/* Beschreibung:
 *     berechnet den Betrag eines Vektors
 * Eingabewerte:
 *     ein Vektor
 * Rueckgabewerte:
 *     den Betrag des Vektors */
double betrag(struct vektor v)
{
    return sqrt(v.x * v.x + v.y * v.y);
}

/* Beschreibung:
 *     berechnet den Abstand zweier Vektoren
 * Eingabewerte:
 *     zwei Vektoren
 * Rueckgabewerte:
 *     Abstand zwischen den Vektoren */
double abstand(struct vektor a, struct vektor b)
{
    struct vektor d;

    /* Verbindungsvektor */
    d.x = b.x - a.x;
    d.y = b.y - a.y;

    /* Betrag des Verbindungsvektors zurueckgeben */
    return betrag(d);
}

/* Beschreibung:
 *     berechnet den Abstand eines Punktes von einer Geraden
 * Eingabewerte:
 *     1. Argument: Ortsvektor des Punktes
 *     2./3. Argument: zwei Punkte der Geraden (Ortsvektoren)
 * Rueckgabewerte:
 *     Abstand des Punktes von der Geraden */
double abstand_punkt_gerade(struct vektor p, struct vektor g1,
                            struct vektor g2)
{
    double m, n;

    /* senkrechte Gerade */
    if (g2.x - g1.x == 0.0)
        return abs(g2.x - p.x); /* Differenz der x-Werte zurueckgeben */

    /* Funktionsgleichung bestimmen */
    m = (g2.y - g1.y) / (g2.x - g1.x);
    n = g1.y - m * g1.x;

    /* waagerechte Gerade */
    if (m == 0.0)
        return abs(g1.y - p.y); /* Differenz der y-Werte zurueckgeben */

    /* Abstand des Punktes von der Geraden zurueckgeben
       (ueber hesse'sche Normalform) */
    return abs((m * p.x - p.y + n) / sqrt(m * m + 1.0));
}

/* Beschreibung:
 *     berechnet das Skalarprodukt zweier Vektoren
 * Eingabewerte:
 *     zwei Vektoren
 * Rueckgabewerte:
 *     das Skalarprodukt der Vektoren */
double skalarprodukt(struct vektor a, struct vektor b)
{
    return (a.x * b.x + a.y * b.y);
}
