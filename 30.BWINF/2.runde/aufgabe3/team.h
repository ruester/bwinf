/* 30. Bundeswettbewerb Informatik - 2. Runde
 * Aufgabe 3 - Trickey
 *
 * Autor: Matthias Rüster
 *
 * E-Mail: ruester@molgen.mpg.de
 */

#ifndef AUFGABE3_TEAM_H
#define AUFGABE3_TEAM_H

struct team {
    char *name;
    int rank, points, goals;
};

extern struct team *team_new(char *name);
extern void team_free(struct team *t);

#endif
