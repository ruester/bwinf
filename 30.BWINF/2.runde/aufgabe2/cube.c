/* 30. Bundeswettbewerb Informatik - 2. Runde
 * Aufgabe 2 - Kool
 *
 * Autor: Matthias Rüster
 *
 * E-Mail: ruester@molgen.mpg.de
 */

#define _GNU_SOURCE

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "cube.h"

struct cube *cube_new(char *type)
{
    struct cube *c;

    if ((c = calloc(1, sizeof(*c))) == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    c->type = convert_type(type);
    cube_init(c);

    return c;
}

struct cube *cube_new_by_type(int type)
{
    struct cube *c;

    if ((c = calloc(1, sizeof(*c))) == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    c->type = type;
    cube_init(c);

    return c;
}

int convert_type(char *type)
{
    if (!strcasecmp("kreuzung", type))
        return KREUZUNG;

    if (!strcasecmp("geradeaus-ost-west", type)
        || !strcasecmp("geradeaus-west-ost", type))
        return GERADEAUS_OST_WEST;

    if (!strcasecmp("geradeaus-nord-sued", type)
        || !strcasecmp("geradeaus-sued-nord", type))
        return GERADEAUS_NORD_SUED;

    if (!strcasecmp("knick-nord-west", type)
        || !strcasecmp("knick-west-nord", type))
        return KNICK_NORD_WEST;

    if (!strcasecmp("knick-nord-ost", type)
        || !strcasecmp("knick-ost-nord", type))
        return KNICK_NORD_OST;

    if (!strcasecmp("knick-ost-sued", type)
        || !strcasecmp("knick-sued-ost", type))
        return KNICK_OST_SUED;

    if (!strcasecmp("knick-sued-west", type)
        || !strcasecmp("knick-west-sued", type))
        return KNICK_SUED_WEST;

    if (!strcasecmp("t-kreuzung-west", type))
        return TKREUZUNG_WEST;

    if (!strcasecmp("t-kreuzung-nord", type))
        return TKREUZUNG_NORD;

    if (!strcasecmp("t-kreuzung-ost", type))
        return TKREUZUNG_OST;

    if (!strcasecmp("t-kreuzung-sued", type))
        return TKREUZUNG_SUED;

    if (!strcasecmp("sackgasse-west", type))
        return SACKGASSE_WEST;

    if (!strcasecmp("sackgasse-nord", type))
        return SACKGASSE_NORD;

    if (!strcasecmp("sackgasse-ost", type))
        return SACKGASSE_OST;

    if (!strcasecmp("sackgasse-sued", type))
        return SACKGASSE_SUED;

    fprintf(stderr, "unbekannter Wuerfeltyp '%s'\n", type);
    exit(EXIT_FAILURE);
}

/* setze Oeffnungen anhand des Typs */
void cube_init(struct cube *c)
{
    c->north = !!(c->type & (1 << 3));
    c->east  = !!(c->type & (1 << 2));
    c->south = !!(c->type & (1 << 1));
    c->west  = !!(c->type & 1);
}

/* um 90 Grad im Uhrzeigersinn drehen; count-mal */
void cube_rotate(struct cube *c, int count)
{
    int h;

    switch (count) {
        case 1:
            h        = c->north;
            c->north = c->west;
            c->west  = c->south;
            c->south = c->east;
            c->east  = h;
        break;

        case 2:
            h        = c->north;
            c->north = c->south;
            c->south = h;
            h        = c->west;
            c->west  = c->east;
            c->east  = h;
        break;

        case 3:
            h        = c->north;
            c->north = c->east;
            c->east  = c->south;
            c->south = c->west;
            c->west  = h;
        break;

        default:
            fprintf(stderr, "bug: ungueltiges Argument count=%d\n",
                    count);
            exit(EXIT_FAILURE);
    }

    c->type = get_type(c);
}

char get_type(struct cube *c)
{
    char type;

    type = 0;

    if (c->north)
        type += (1 << 3);

    if (c->east)
        type += (1 << 2);

    if (c->south)
        type += (1 << 1);

    if (c->west)
        type += 1;

    return type;
}

char *type_to_string(struct cube *c)
{
    char *ret;

    switch (c->type) {
        case GERADEAUS_OST_WEST:
            asprintf(&ret, "Geradeaus-Ost-West");
            break;

        case GERADEAUS_NORD_SUED:
            asprintf(&ret, "Geradeaus-Nord-Sued");
            break;

        case KNICK_NORD_WEST:
            asprintf(&ret, "Knick-Nord-West");
            break;

        case KNICK_NORD_OST:
            asprintf(&ret, "Knick-Nord-Ost");
            break;

        case KNICK_OST_SUED:
            asprintf(&ret, "Knick-Ost-Sued");
            break;

        case KNICK_SUED_WEST:
            asprintf(&ret, "Knick-Sued-West");
            break;

        case KREUZUNG:
            asprintf(&ret, "Kreuzung");
            break;

        case TKREUZUNG_WEST:
            asprintf(&ret, "T-Kreuzung-West");
            break;

        case TKREUZUNG_NORD:
            asprintf(&ret, "T-Kreuzung-Nord");
            break;

        case TKREUZUNG_OST:
            asprintf(&ret, "T-Kreuzung-Ost");
            break;

        case TKREUZUNG_SUED:
            asprintf(&ret, "T-Kreuzung-Sued");
            break;

        case SACKGASSE_WEST:
            asprintf(&ret, "Sackgasse-West");
            break;

        case SACKGASSE_NORD:
            asprintf(&ret, "Sackgasse-Nord");
            break;

        case SACKGASSE_OST:
            asprintf(&ret, "Sackgasse-Ost");
            break;

        case SACKGASSE_SUED:
            asprintf(&ret, "Sackgasse-Sued");
            break;

        default:
            fprintf(stderr, "bug: unbekannter Typ %d\n", c->type);
            exit(EXIT_FAILURE);
    }

    return ret;
}
