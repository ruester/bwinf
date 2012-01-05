/* 30. Bundeswettbewerb Informatik - 2. Runde
 * Aufgabe 2 - Kool
 *
 * Autor: Matthias Rüster
 *
 * E-Mail: ruester@molgen.mpg.de
 */

#ifndef AUFGABE2_RECTANGLE_H
#define AUFGABE2_RECTANGLE_H

#include "cube.h"

struct rectangle {
    /* zweidimensionales Array von Wuerfeln */
    struct cube **cube;

    /* Breite und Hoehe des Arrays */
    int width, height;

    /* Anzahl der zur Verfuegung stehenden Wuerfel */
    int cube_cnt;

    /* Anzahl der Fehler */
    int faults;
};

extern struct rectangle *rectangle_new(int height, int width, int count);
extern void rectangle_remove(struct rectangle *r);
extern void rectangle_evaluate(struct rectangle *r, char alley);

#endif
