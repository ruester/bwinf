/* 30. Bundeswettbewerb Informatik - 2. Runde
 * Aufgabe 3 - Trickey
 *
 * Autor: Matthias Rüster
 *
 * E-Mail: ruester@molgen.mpg.de
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "team.h"

struct team *team_new(char *name)
{
    struct team *t;

    if ((t = calloc(1, sizeof(*t))) == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    t->name = strdup(name);

    return t;
}

void team_free(struct team *t)
{
    if (!t)
        return;

    free(t->name);
    free(t);
}
