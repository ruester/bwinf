/* 30. Bundeswettbewerb Informatik - 2. Runde
 * Aufgabe 2 - Kool
 *
 * Autor: Matthias Rüster
 *
 * E-Mail: ruester@molgen.mpg.de
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cube.h"
#include "rectangle.h"

struct rectangle *rectangle_new(int height, int width, int count)
{
    struct rectangle *r;
    int i;

    if ((r = calloc(1, sizeof(*r))) == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    if ((r->cube = calloc(height, sizeof(*(r->cube)))) == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < height; i++) {
        if ((r->cube[i] = calloc(width, sizeof(*(r->cube[i])))) == NULL) {
            perror("calloc");
            exit(EXIT_FAILURE);
        }
    }

    r->height   = height;
    r->width    = width;
    r->cube_cnt = count;

    return r;
}

void rectangle_remove(struct rectangle *r)
{
    int i;

    for (i = 0; i < r->height; i++)
       free(r->cube[i]);

    free(r->cube);
    free(r);
}

/* rekursive Funktion: zaehlt alle erreichbaren Wuerfel.
   -> besuchte Wuerfel, die falsche Eigenschaften haben (Ausgaenge
   an den Raendern, kein richtiger "Startwuerfel")
   erhoehen den Fehlerwert */
void recursive_check(struct rectangle *r, char **visit, int x, int y,
                     int *visited)
{
    if (visit[y][x])
        /* Wuerfel wurde schonmal besucht */
        return;

    /* eintragen: Wuerfel wurde besucht */
    visit[y][x] = 1;
    (*visited)++;

    /* nach links gehen, wenn moeglich */
    if (x != 0 && r->cube[y][x].west
               && r->cube[y][x - 1].east)
        recursive_check(r, visit, x - 1, y, visited);

    /* nach rechts gehen, wenn moeglich */
    if (x != r->width - 1 && r->cube[y][x].east
                          && r->cube[y][x + 1].west)
        recursive_check(r, visit, x + 1, y, visited);

    /* nach oben gehen, wenn moeglich */
    if (y != 0 && r->cube[y][x].north
               && r->cube[y - 1][x].south)
        recursive_check(r, visit, x, y - 1, visited);

    /* nach unten gehen, wenn moeglich */
    if (y != r->height - 1 && r->cube[y][x].south
                           && r->cube[y + 1][x].north)
        recursive_check(r, visit, x, y + 1, visited);
}

/* ermittle Fehler des Labyrinths */
void rectangle_evaluate(struct rectangle *r, char alley)
{
    char **visit;
    int x, y, count, old_faults;

    if ((visit = calloc(r->height, sizeof(*visit))) == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    for (y = 0; y < r->height; y++) {
        if ((visit[y] = calloc(r->width, sizeof(*(visit[y])))) == NULL) {
            perror("calloc");
            exit(EXIT_FAILURE);
        }
    }

    count = 0;

    /* durchlaufe das Labyrinth; besuchte Wuerfel merken und zaehlen */
    recursive_check(r, visit, 0, 0, &count);

    for (y = 0; y < r->height; y++)
        free(visit[y]);
    free(visit);

    old_faults = r->faults;

    /* alle nicht erreichten Wuerfel sind Fehler */
    r->faults = r->cube_cnt - count;

    if (old_faults < r->faults)
        return; /* schlechtere Ergebnisse werden immer verworfen */

    for (y = 0; y < r->height && old_faults >= r->faults; y++) {
        for (x = 0; x < r->width && old_faults >= r->faults; x++) {
            if (x == 0 && y == 0) {
                if (r->cube[0][0].west
                    && r->cube[0][0].north)
                    r->faults++;

                if (!r->cube[0][0].west
                    && !r->cube[0][0].north)
                    r->faults++;

                continue;
            }

            /* Raender pruefen */
            if (x == r->width - 1 && r->cube[y][x].east)
                r->faults++;

            if (x == 0 && r->cube[y][x].west)
                r->faults++;

            if (y == 0 && r->cube[y][x].north)
                r->faults++;

            if (y == r->height - 1 && r->cube[y][x].south)
                r->faults++;

            if (alley) {
                if (r->cube[y][x].type != LEER)
                    continue;

                /* keine Wege in "leere Wuerfel" enden lassen */
                if (y != r->height - 1 && r->cube[y + 1][x].north)
                    r->faults++;

                if (x != r->width - 1 && r->cube[y][x + 1].west)
                    r->faults++;

                if (y != 0 && r->cube[y - 1][x].south)
                    r->faults++;

                if (x != 0 && r->cube[y][x - 1].east)
                    r->faults++;

                continue;
            }

            /* unten pruefen */
            if (y != r->height - 1
                && r->cube[y][x].south != r->cube[y + 1][x].north)
                r->faults++;

            /* rechts pruefen */
            if (x != r->width - 1
                && r->cube[y][x].east != r->cube[y][x + 1].west)
                r->faults++;
        }
    }
}
