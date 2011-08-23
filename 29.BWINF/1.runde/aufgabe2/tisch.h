/* Bundeswettbewerb Informatik
 * Aufgabe 2 - Robuttons
 *
 * Autor: Matthias Rüster
 *
 * E-Mail: ruester@molgen.mpg.de
 */

#ifndef AUFGABE2_TISCH_H
#define AUFGABE2_TISCH_H

/* Tischtypen */
#define KREIS   0
#define VIERECK 1
#define POLYGON 2

struct tisch {
    struct vektor *ecken;       /* Koordinaten der Ecken des Tisches */
    int anzahl_ecken;           /* Anzahl der Ecken */
    char typ;                   /* Tischtyp */
};

extern struct tisch *neuer_poly_tisch(struct vektor *ecken,
                                      int anzahl_ecken);

extern struct tisch *neuer_runder_tisch(struct vektor mittelpunkt,
                                        double radius);

extern struct tisch *neuer_viereckiger_tisch(struct vektor v1,
                                             struct vektor v2);

extern void tisch_freigeben(struct tisch *t);

extern char ist_tischflaeche(struct tisch *t, double x, double y);

#endif
