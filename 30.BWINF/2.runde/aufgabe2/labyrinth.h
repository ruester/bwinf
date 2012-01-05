/* 30. Bundeswettbewerb Informatik - 2. Runde
 * Aufgabe 2 - Kool
 *
 * Autor: Matthias Rüster
 *
 * E-Mail: ruester@molgen.mpg.de
 */

#ifndef AUFGABE2_LABYRINTH_H
#define AUFGABE2_LABYRINTH_H

#include "cube.h"
#include "rectangle.h"

struct labyrinth {
    /* alle zur Verfuegung stehenden Wuerfel
       (Wuerfelmenge) */
    struct cube **cubes;
    int cube_cnt;

    /* welche Groesse soll "untersucht" werden? */
    int width, height;

    /* wie lange soll probiert werden? */
    int try_count;

    char verbose,   /* mehr Output? */
         rotation,  /* Rotation erlaubt? */
         alley;     /* Art der Sackgassen */

    /* Loesung */
    struct rectangle *solution;
};

struct sizes {
    int *width, *height;
    int count;
};

extern struct labyrinth *labyrinth_load(char *filename);
extern void add_size(struct sizes *s, int width, int height);
extern void init_sizes(struct sizes *s, int cube_cnt);
extern void labyrinth_solve(struct labyrinth *l);
extern void labyrinth2png(struct rectangle *r, char *filename);
extern void labyrinth_remove(struct labyrinth *l);
extern void labyrinth2file(struct labyrinth *l, char *filename);
extern struct labyrinth *labyrinth_generate(int width, int height);
extern char is_solvable(struct labyrinth *l);

#endif
