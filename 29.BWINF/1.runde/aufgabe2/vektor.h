/* Bundeswettbewerb Informatik
 * Aufgabe 2 - Robuttons
 *
 * Autor: Matthias Rüster
 *
 * E-Mail: ruester@molgen.mpg.de
 */

#ifndef AUFGABE2_VEKTOR_H
#define AUFGABE2_VEKTOR_H

struct vektor {
    double x, y;
};

struct strecke {
    struct vektor anfang, ende;
};

extern double abstand_punkt_gerade(struct vektor p,
                                   struct vektor g1, struct vektor g2);

extern double skalarprodukt(struct vektor a, struct vektor b);

extern double betrag(struct vektor v);

extern double abstand(struct vektor a, struct vektor b);

extern struct vektor *neue_vektoren(int anzahl);

extern struct vektor *schnittpunkt_strecke_kreis(struct strecke a,
                                                 struct vektor mittelpunkt,
                                                 double r);

extern struct vektor *schnittpunkt_strecken(struct strecke a,
                                            struct strecke b);

extern struct vektor *schnittpunkt_strecke_strahl(struct strecke a,
                                                  struct strecke b);

#endif
