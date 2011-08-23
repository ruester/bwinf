/* Bundeswettbewerb Informatik
 * Aufgabe 2 - Robuttons
 *
 * Autor: Matthias Rüster
 *
 * E-Mail: ruester@molgen.mpg.de
 */

#include <math.h>
#include <stdlib.h>

#include "simulation.h"
#include "kollision.h"
#include "vektor.h"
#include "robutton.h"
#include "tisch.h"

/* Beschreibung:
 *     prueft, ob ein Robutton mit einer Tischkante (eckiger Tisch)
 *     kollidieren wuerde und behandelt die Kollision
 * Eingabewerte:
 *     1. Argument: Zeiger auf die Simulationsstruktur
 *     2. Argument: Index des zu untersuchenden Robuttons
 * Rueckgabewerte:
 *     0, wenn der Robutton nicht kollidiert ist
 *     1, wenn der Robutton kollidiert ist */
char pruefe_kollision_tischkante_eckig(struct simulation *s, int i)
{
    int j;
    double d, m, delta_x, z, l;
    struct strecke st, kante;
    struct vektor u, a, f;
    struct vektor *schnittpunkt;

    /* alle Tischkanten durchlaufen */
    for (j = 0; j < s->tisch->anzahl_ecken - 1; j++) {
        /* Strecke des Robuttons */
        st.anfang = s->robuttons[i].position;
        st.ende = s->neue_positionen[i];

        /* Strecke der Tischkante */
        kante.anfang = s->tisch->ecken[j];
        kante.ende = s->tisch->ecken[j + 1];

        /* Bedingungen fuer eine Kollision */
        if ((d =
             abstand_punkt_gerade(s->neue_positionen[i],
                                  s->tisch->ecken[j],
                                  s->tisch->ecken[j + 1])) <= 1.0
            && (schnittpunkt = schnittpunkt_strecken(kante, st)) != NULL) {
            delta_x = s->tisch->ecken[j + 1].x - s->tisch->ecken[j].x;

            /* vertikale Gerade */
            if (delta_x == 0) {
                /* x-Richtung wird umgekehrt */
                s->robuttons[i].richtung.x *= (-1);

                /* neue Position berechnen */
                if (s->robuttons[i].richtung.x > 0.0)
                    s->neue_positionen[i].x =
                        s->tisch->ecken[j].x
                        + s->robuttons[i].richtung.x - d;
                else
                    s->neue_positionen[i].x =
                        s->tisch->ecken[j].x
                        + s->robuttons[i].richtung.x + d;
            } else {
                /* Anstieg der Gerade bestimmen */
                m = (s->tisch->ecken[j + 1].y -
                     s->tisch->ecken[j].y) / delta_x;

                /* horizontale Gerade */
                if (m == 0.0) {
                    /* y-Richtung wird umgekehrt */
                    s->robuttons[i].richtung.y *= (-1.0);

                    /* neue Position berechnen */
                    if (s->robuttons[i].richtung.y > 0.0)
                        s->neue_positionen[i].y =
                            s->tisch->ecken[j].y +
                            s->robuttons[i].richtung.y - d;
                    else
                        s->neue_positionen[i].y =
                            s->tisch->ecken[j].y +
                            s->robuttons[i].richtung.y + d;
                } else {
                    /* diagonale/schraege Gerade */
                    /* Punkt ausserhalb der Tischflaeche wird
                     * an der Tischkante gespiegelt */
                    u.x = kante.ende.x - kante.anfang.x;
                    u.y = kante.ende.y - kante.anfang.y;

                    a.x = s->neue_positionen[i].x - kante.anfang.x;
                    a.y = s->neue_positionen[i].y - kante.anfang.y;

                    l = skalarprodukt(a, u) / skalarprodukt(u, u);

                    f.x = kante.anfang.x + l * u.x;
                    f.y = kante.anfang.y + l * u.y;

                    /* neue Position berechnen */
                    s->neue_positionen[i].x =
                        (-1.0) * s->neue_positionen[i].x + 2.0 * f.x;
                    s->neue_positionen[i].y =
                        (-1.0) * s->neue_positionen[i].y + 2.0 * f.y;

                    /* neue Richtung berechnen */
                    s->robuttons[i].richtung.x =
                        s->neue_positionen[i].x - schnittpunkt->x;
                    s->robuttons[i].richtung.y =
                        s->neue_positionen[i].y - schnittpunkt->y;

                    /* Richtungsvektor zum Einheitsvektor machen */
                    z = betrag(s->robuttons[i].richtung);

                    s->robuttons[i].richtung.x /= z;
                    s->robuttons[i].richtung.y /= z;
                }
            }

            /* Speicher freigeben */
            free(schnittpunkt);

            return (1);
        }
    }

    return (0);
}

/* Beschreibung:
 *     prueft, ob ein Robutton mit einer Tischkante (runder Tisch)
 *     kollidieren wuerde und behandelt die Kollision
 * Eingabewerte:
 *     1. Argument: Zeiger auf die Simulationsstruktur
 *     2. Argument: Index des zu untersuchenden Robuttons
 * Rueckgabewerte:
 *     0, wenn der Robutton nicht kollidiert ist
 *     1, wenn der Robutton kollidiert ist */
char pruefe_kollision_tischkante_rund(struct simulation *s, int i)
{
    double m, m2, n2, r, delta_x, l, z, d;
    struct vektor v, a, b, u, f;
    struct vektor *schnittpunkt;
    struct strecke st;

    /* Mittelpunkt */
    v.x = s->tisch->ecken[0].x;
    v.y = s->tisch->ecken[0].y;

    /* Radius */
    r = s->tisch->ecken[1].x;

    /* Strecke des Robuttons */
    st.anfang = s->robuttons[i].position;
    st.ende   = s->neue_positionen[i];

    /* Bedingungen fuer die Kollision */
    if (!ist_tischflaeche(s->tisch,
                          s->neue_positionen[i].x, s->neue_positionen[i].y)
        && (schnittpunkt = schnittpunkt_strecke_kreis(st, v, r)) != NULL) {
        
        /* Tangente im Schnittpunkt ermitteln */
        
        delta_x = schnittpunkt->x - v.x;

        /* horizontale Tangente */
        if (delta_x == 0.0) {
            /* y-Richtung umkehren */
            s->robuttons[i].richtung.y *= (-1.0);

            /* zwei Punkte der Tangente bestimmen */
            a.x = schnittpunkt->x;
            a.y = schnittpunkt->y;

            b.x = schnittpunkt->x + 1.0;
            b.y = schnittpunkt->y;

            /* Abstand des Robuttons zur Tangente berechnen */
            d = abstand_punkt_gerade(s->neue_positionen[i], a, b);

            /* neue Position berechnen */
            if (s->robuttons[i].richtung.y > 0.0)
                s->neue_positionen[i].y =
                    schnittpunkt->y + s->robuttons[i].richtung.y - d;
            else
                s->neue_positionen[i].y =
                    schnittpunkt->y + s->robuttons[i].richtung.y + d;
        } else {
            /* Anstieg der Geraden vom Mittelpunkt
             * zum Schnittpunkt bestimmen */
            m = (schnittpunkt->y - v.y) / delta_x;

            /* vertikale Tangente */
            if (m == 0.0) {
                /* x-Richtung umkehren */
                s->robuttons[i].richtung.x *= (-1);

                /* zwei Punkte der Tangente bestimmen */
                a.x = schnittpunkt->x;
                a.y = schnittpunkt->y;

                b.x = schnittpunkt->x;
                b.y = schnittpunkt->y + 1.0;

                /* Abstand des Robuttons zur Tangente berechnen */
                d = abstand_punkt_gerade(s->neue_positionen[i], a, b);

                /* neue Position berechnen */
                if (s->robuttons[i].richtung.x > 0.0)
                    s->neue_positionen[i].x =
                        schnittpunkt->x + s->robuttons[i].richtung.x - d;
                else
                    s->neue_positionen[i].x =
                        schnittpunkt->x + s->robuttons[i].richtung.x + d;
            } else {
                /* Gleichung der Tangenten ermitteln */
                m2 = (-1.0) / m;
                n2 = schnittpunkt->y - m2 * schnittpunkt->x;

                /* Punkt ausserhalb der Tischflaeche wird
                 * an der Tangente gespiegelt */
                u.x = 2.0 - 1.0;
                u.y = (m2 * 2.0 + n2) - (m2 * 1.0 + n2);

                a.x = s->neue_positionen[i].x - 1.0;
                a.y = s->neue_positionen[i].y - (m2 * 1.0 + n2);

                l = skalarprodukt(a, u) / skalarprodukt(u, u);

                f.x = 1.0 + l * u.x;
                f.y = (m2 * 1.0 + n2) + l * u.y;

                /* neue Position berechnen */
                s->neue_positionen[i].x =
                    (-1.0) * s->neue_positionen[i].x + 2.0 * f.x;
                s->neue_positionen[i].y =
                    (-1.0) * s->neue_positionen[i].y + 2.0 * f.y;

                /* neue Richtung berechnen */
                s->robuttons[i].richtung.x =
                    s->neue_positionen[i].x - schnittpunkt->x;
                s->robuttons[i].richtung.y =
                    s->neue_positionen[i].y - schnittpunkt->y;

                /* Richtungsvektor des Robuttons zum Einheitsvektor machen */
                z = betrag(s->robuttons[i].richtung);
                
                s->robuttons[i].richtung.x /= z;
                s->robuttons[i].richtung.y /= z;
            }
        }

        free(schnittpunkt);

        return (1);
    }

    return (0);
}

/* Beschreibung:
 *     prueft, ob ein Robutton mit einem anderen Robutton kollidieren
 *     wuerde und behandelt die Kollision
 * Eingabewerte:
 *     1. Argument: Zeiger auf die Simulationsstruktur
 *     2. Argument: Index des zu untersuchenden Robuttons
 * Rueckgabewerte:
 *     0, wenn der Robutton nicht kollidiert ist
 *     1, wenn der Robutton kollidiert ist */
char pruefe_kollision_robuttons(struct simulation *s, int i)
{
    int j;
    double b;

    for (j = 0; j < s->anzahl_robuttons; j++) {
        if (i == j)
            continue;
        
        /* Kollision mit anderem Robutton pruefen */
        if (abstand(s->neue_positionen[i], s->neue_positionen[j]) < 1.0) {
            /* neue Richtungen der Robuttons bestimmen */
            b = acos(s->robuttons[i].richtung.x);
            
            if(s->robuttons[i].richtung.y < 0)
                b = 2 * M_PI - b;
            
            b += M_PI / 2.0 + M_PI * drand48();

            s->robuttons[i].richtung.x = cos(b);
            s->robuttons[i].richtung.y = sin(b);

            b = acos(s->robuttons[j].richtung.x);
            
            if(s->robuttons[j].richtung.y < 0)
                b = 2 * M_PI - b;
            
            b += M_PI / 2.0 + M_PI * drand48();

            s->robuttons[j].richtung.x = cos(b);
            s->robuttons[j].richtung.y = sin(b);

            /* neue Positionen der Robuttons bestimmen */
            s->neue_positionen[i].x =
                s->robuttons[i].position.x + s->robuttons[i].richtung.x;
            s->neue_positionen[i].y =
                s->robuttons[i].position.y + s->robuttons[i].richtung.y;

            s->neue_positionen[j].x =
                s->robuttons[j].position.x + s->robuttons[j].richtung.x;
            s->neue_positionen[j].y =
                s->robuttons[j].position.y + s->robuttons[j].richtung.y;
            
            /* aufwendige Kollisionspruefung des 2. Robuttons vermeiden */
            if (!ist_tischflaeche(s->tisch, s->neue_positionen[j].x,
                                  s->neue_positionen[j].y)) {
                s->neue_positionen[j].x = s->robuttons[j].position.x;
                s->neue_positionen[j].y = s->robuttons[j].position.y;
            }

            return (1);
        }
    }

    return (0);
}

/* Beschreibung:
 *     prueft, ob ein Robutton mit einer Muenze kollidieren
 *     wuerde und behandelt die Kollision
 * Eingabewerte:
 *     1. Argument: Zeiger auf die Simulationsstruktur
 *     2. Argument: Index des zu untersuchenden Robuttons
 * Rueckgabewerte:
 *     0, wenn der Robutton nicht kollidiert ist
 *     1, wenn der Robutton kollidiert ist */
char pruefe_kollision_muenzen(struct simulation *s, int i)
{
    int j, k;
    double b;

    /* alle Muenzen durchlaufen */
    for (j = 0; j < s->anzahl_muenzen; j++) {
        if (s->muenzen[j].x == -1.0 && s->muenzen[j].y == -1.0)
            continue; /* Muenze wird von einem Robutton getragen */

        /* Bedingung fuer die Kollision */
        if ((int) s->neue_positionen[i].x == (int) s->muenzen[j].x
            && (int) s->neue_positionen[i].y == (int) s->muenzen[j].y) {
            
            /* Robutton traegt eine Muenze */
            if (s->robuttons[i].hat_muenze) {
                /* Muenze fallen lassen */
                s->robuttons[i].hat_muenze = 0;

                /* Muenze auf dem Tisch platzieren */
                for (k = 0; k < s->anzahl_muenzen; k++) {
                    if (s->muenzen[k].x == -1.0 && s->muenzen[k].y == -1.0) {
                        s->muenzen[k].x = (int) s->robuttons[i].position.x;
                        s->muenzen[k].y = (int) s->robuttons[i].position.y;
                        break;
                    }
                }

                /* Robutton drehen */
                b = acos(s->robuttons[i].richtung.x);
                
                if(s->robuttons[i].richtung.y < 0)
                    b = 2 * M_PI - b;
                
                b += M_PI;

                /* neue Richtung berechnen */
                s->robuttons[i].richtung.x = cos(b);
                s->robuttons[i].richtung.y = sin(b);

                /* neue Position berechnen */
                s->neue_positionen[i].x =
                    s->robuttons[i].position.x + s->robuttons[i].richtung.x;
                s->neue_positionen[i].y =
                    s->robuttons[i].position.y + s->robuttons[i].richtung.y;

                /* Position des Robuttons hat sich geaendert */
                return (1);
            } else {
                /* Robutton nimmt Muenze auf */
                s->robuttons[i].hat_muenze = 1;

                /* Muenze vom Tisch nehmen */
                s->muenzen[j].x = -1.0;
                s->muenzen[j].y = -1.0;
            }
        }
    }

    /* Position des Robutton hat sich nicht geaendert */
    return (0);
}
