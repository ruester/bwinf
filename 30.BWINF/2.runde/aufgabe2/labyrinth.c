/* 30. Bundeswettbewerb Informatik - 2. Runde
 * Aufgabe 2 - Kool
 *
 * Autor: Matthias Rüster
 *
 * E-Mail: ruester@molgen.mpg.de
 */

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <gd.h>

#include "rectangle.h"
#include "labyrinth.h"
#include "cube.h"

#define RGB_WHITE  255, 255, 255
#define RGB_BLACK    0,   0,   0
#define RGB_YELLOW 255, 255,   0
#define RGB_GRAY   100, 100, 100

/* entferne fuehrende und angehaengte whitespaces */
static char *trim(char *s)
{
    char *e;
    int len;

    len = strlen(s);
    e   = s + len - 1;

    while (isspace(*e)) {
        *e = '\0';
        e--;
    }

    while (isspace(*s)) {
        *s = '\0';
        s++;
    }

    return s;
}

void add_size(struct sizes *s, int width, int height)
{
    int i;

    for (i = 0; i < s->count; i++)
        if (s->width[i] == width && s->height[i] == height)
            return;


    if ((s->width = realloc(s->width,
                            (s->count + 1) * sizeof(int *))) == NULL
        || (s->height = realloc(s->height,
                            (s->count + 1) * sizeof(int *))) == NULL) {
        perror("realloc");
        exit(EXIT_FAILURE);
    }

    s->width[s->count]  = width;
    s->height[s->count] = height;
    s->count++;
}

void shuffle_sizes(struct sizes *s)
{
    int i, a, b, h1, h2;

    for (i = 0; i < 2 * s->count; i++) {
        a = rand() % s->count;
        b = rand() % s->count;

        h1 = s->height[a];
        h2 = s->width[a];

        s->height[a] = s->height[b];
        s->width[a]  = s->width[b];

        s->height[b] = h1;
        s->width[b]  = h2;
    }
}

/* ermittle optimale Rechtecksflaechen */
void init_sizes(struct sizes *s, int cube_cnt)
{
    int width, height, i, over, c;

    width  = cube_cnt;
    height = 1;

    while (width > 0) {
        height = ceil((double) cube_cnt / (double) width);

        if (width < height)
            break;

        over = width * height - cube_cnt;

        if (over < height && over < width)
            add_size(s, width, height);

        width--;
    }

    c = s->count;

    for (i = 0; i < c; i++)
        add_size(s, s->height[i], s->width[i]);

    shuffle_sizes(s);
}

static struct labyrinth *labyrinth_new(void)
{
    struct labyrinth *l;

    if ((l = calloc(1, sizeof(*l))) == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    return l;
}

void labyrinth_remove(struct labyrinth *l)
{
    int i;

    for (i = 0; i < l->cube_cnt; i++)
        free(l->cubes[i]);
    free(l->cubes);

    if (l->solution)
        rectangle_remove(l->solution);

    free(l);
}

static void add_cube(struct labyrinth *l, char *type)
{
    l->cubes = realloc(l->cubes,
                       (l->cube_cnt + 1) * sizeof(*(l->cubes)));

    if (l->cubes == NULL) {
        perror("realloc");
        exit(EXIT_FAILURE);
    }

    l->cubes[l->cube_cnt] = cube_new(type);
    l->cube_cnt++;
}

static void add_cube_by_type(struct labyrinth *l, int type)
{
    l->cubes = realloc(l->cubes,
                       (l->cube_cnt + 1) * sizeof(*(l->cubes)));

    if (l->cubes == NULL) {
        perror("realloc");
        exit(EXIT_FAILURE);
    }

    l->cubes[l->cube_cnt] = cube_new_by_type(type);
    l->cube_cnt++;
}

/* Labyrinth aus Datei einlesen */
struct labyrinth *labyrinth_load(char *filename)
{
    FILE *file;
    char line[LINE_MAX];
    char *count, *type;
    char delim[] = " ,;#|";
    struct labyrinth *lab;
    int line_cnt, i, c;

    if ((file = fopen(filename, "r")) == NULL) {
        fprintf(stderr, "%s: ", filename);
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    lab      = labyrinth_new();
    line_cnt = 0;

    while (fgets(line, LINE_MAX, file) != NULL) {
        line_cnt++;

        trim(line);

        if ((count = strtok(line, delim)) == NULL
            || (type = strtok(NULL, delim)) == NULL) {
            fprintf(stderr, "%s: Zeile %d konnte nicht eingelesen werden\n",
                    filename, line_cnt);
            exit(EXIT_FAILURE);
        }

        if ((c = atoi(count)) < 0) {
            fprintf(stderr, "%s: Zeile %d: Anzahl muss groesser 0 sein\n",
                    filename, line_cnt);
            exit(EXIT_FAILURE);
        }

        for (i = 0; i < c; i++)
            add_cube(lab, type);
    }

    if (fclose(file) == EOF) {
        perror("fclose");
        exit(EXIT_FAILURE);
    }

    if (lab->cube_cnt <= 1) {
        fprintf(stderr,
                "Das Labyrinth muss mindestens "
                "aus 2 Wuerfeln bestehen\n");
        exit(EXIT_FAILURE);
    }

    return lab;
}

/* "befuelle" das Rechteck mit den zur Verfuegung stehenden Wuerfeln */
void rectangle_fill(struct labyrinth *l, struct rectangle *r)
{
    int x, y, i;

    for (y = 0; y < r->height; y++)
        for (x = 0; x < r->width; x++)
            r->cube[y][x].type = LEER;

    for (i = 0; i < l->cube_cnt; i++) {
        do {
            x = rand() % l->width;
            y = rand() % l->height;
        } while (r->cube[y][x].type != LEER);

        r->cube[y][x] = *(l->cubes[i]);
    }

    r->faults = INT_MAX;
    rectangle_evaluate(r, l->alley);
}

/* suche gueltiges Labyrinth */
void labyrinth_solve(struct labyrinth *l)
{
    struct cube c;
    char rot, rot_count;
    int x_from, y_from, x_to, y_to,
        old_faults, i;
    struct rectangle *r;

    printf("suche %4d x %-4d Labyrinth\n", l->width, l->height);

    r = rectangle_new(l->height, l->width, l->cube_cnt);
    rectangle_fill(l, r);

    rot_count = -1;

    for (i = 0; i < l->try_count; i++) {
        x_from = x_to = rand() % r->width;
        y_from = y_to = rand() % r->height;

        rot = 0;

        if (l->rotation && rand() % 2) {
            rot       = 1;
            rot_count = 1 + rand() % 3;
        }

        while (!rot && x_from == x_to && y_from == y_to
               && r->cube[y_from][x_from].type == r->cube[y_to][x_to].type) {
            x_to = rand() % r->width;
            y_to = rand() % r->height;
        }

        old_faults = r->faults;

        if (rot) {
            cube_rotate(&(r->cube[y_from][x_from]), rot_count);
        } else {
            c                       = r->cube[y_from][x_from];
            r->cube[y_from][x_from] = r->cube[y_to][x_to];
            r->cube[y_to][x_to]     = c;
        }

        rectangle_evaluate(r, l->alley);

        if (r->faults == 0) {
            l->solution = r;
            return;
        }

        if (r->faults <= old_faults)
            continue;

        r->faults = old_faults;

        if (rot) {
            /* wieder zurueck drehen */
            cube_rotate(&(r->cube[y_from][x_from]), 4 - rot_count);
        } else {
            /* wieder zurueck tauschen */
            c                       = r->cube[y_from][x_from];
            r->cube[y_from][x_from] = r->cube[y_to][x_to];
            r->cube[y_to][x_to]     = c;
        }
    }

    if (l->verbose)
        printf("Labyrinth %4d x %-4d hat noch %d Fehler\n",
               l->width, l->height, r->faults);

    rectangle_remove(r);
}

void draw_block(gdImagePtr g, int x, int y, int size, int color)
{
    gdPoint block[4];

    block[0].x = x;
    block[0].y = y;
    block[1].x = x + size;
    block[1].y = y;
    block[2].x = x + size;
    block[2].y = y + size;
    block[3].x = x;
    block[3].y = y + size;

    gdImageFilledPolygon(g, block, 4, color);
}

void draw_cube(gdImagePtr g, struct cube *c, int x, int y, int edge,
               int yellow, int gray, int black)
{
    int size, color;

    size = edge / 3;

    /* Ecken zeichnen */
    draw_block(g, x,            y,            size, gray);
    draw_block(g, x + 2 * size, y,            size, gray);
    draw_block(g, x,            y + 2 * size, size, gray);
    draw_block(g, x + 2 * size, y + 2 * size, size, gray);

    color = yellow;

    /* Mitte zeichnen */
    if (c->type == LEER)
        color = gray;

    draw_block(g, x + size, y + size, size, color);

    color = gray;

    if (c->north)
        color = yellow;

    draw_block(g, x + size, y, size, color);

    color = gray;

    if (c->east)
        color = yellow;

    draw_block(g, x + 2 * size, y + size, size, color);

    color = gray;

    if (c->south)
        color = yellow;

    draw_block(g, x + size, y + 2 * size, size, color);

    color = gray;

    if (c->west)
        color = yellow;

    draw_block(g, x, y + size, size, color);

    /* Raender zeichnen */
    gdImageRectangle(g, x, y, x + edge, y + edge, black);
}

/* Labyrinth als png-Bild abspeichern */
void labyrinth2png(struct rectangle *r, char *filename)
{
    FILE *png;
    gdImagePtr image;
    int white, yellow, gray, black, edge, width, height, x, y;

    edge = 210;

    width  = r->width  * edge;
    height = r->height * edge;

    if ((png = fopen(filename, "w")) == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    if ((image = gdImageCreate(width + 1, height + 1)) == NULL) {
        perror("gdImageCreate");
        exit(EXIT_FAILURE);
    }

    /* Farben initialisieren */
    white  = gdImageColorAllocate(image, RGB_WHITE);
    yellow = gdImageColorAllocate(image, RGB_YELLOW);
    gray   = gdImageColorAllocate(image, RGB_GRAY);
    black  = gdImageColorAllocate(image, RGB_BLACK);

    gdImageFill(image, width, height, white);

    for (y = 0; y < r->height; y++)
        for (x = 0; x < r->width; x++)
            if (r->cube[y][x].type != LEER)
                draw_cube(image, &(r->cube[y][x]),
                          x * edge, y * edge, edge,
                          yellow, gray, black);

    gdImagePng(image, png);
    fclose(png);
    gdImageDestroy(image);
}

/* zaehle alle Wuerfel eines bestimmten Typs */
int count_cube(struct labyrinth *l, int type)
{
    int i, c;

    c = 0;

    for (i = 0; i < l->cube_cnt; i++)
        if (l->cubes[i]->type == type)
            c++;

    return c;
}

/* Labyrinth als Textdatei abspeichern */
void labyrinth2file(struct labyrinth *l, char *filename)
{
    int i;
    char *type;
    char printed[TYPE_CNT] = {0};
    FILE *file;

    if ((file = fopen(filename, "w")) == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < l->cube_cnt; i++) {
        if (printed[(int) l->cubes[i]->type])
            continue;

        printed[(int) l->cubes[i]->type] = 1;
        type = type_to_string(l->cubes[i]);

        fprintf(file, "%d %s\n", count_cube(l, l->cubes[i]->type), type);
        free(type);
    }

    if (fclose(file) == EOF) {
        perror("fclose");
        exit(EXIT_FAILURE);
    }
}

/* pruefen, ob auf Koordinate x, y ein leeres Feld ist */
static int is_empty(struct rectangle *r, int x, int y)
{
    if (x < 0 || y < 0 || x > r->width - 1 || y > r->height - 1)
        return 0;

    return r->cube[y][x].type == LEER;
}

/* rekursive Funktion zum Erzeugen eines zufaelligen Labyrinths */
static void random_fill_rectangle(struct rectangle *r,
                                  int x, int y, int count)
{
    int i, x_n, y_n, dir;

    /* zufaellige Richtung */
    dir = rand() % DIRECTIONS;

    /* Spezialfall: Oeffnung festlegen */
    if (x == 0 && y == 0)
        r->cube[0][0].west = 1;

    /* in jede Richtung gehen */
    for (i = 0; i < 4; i++) {
        if (dir == DIRECTIONS)
            dir = 0;

        switch (dir) {
            case EAST: /* gehe nach rechts */
                x_n = x + 1;
                y_n = y;
            break;
            
            case WEST: /* gehe nach links */
                x_n = x - 1;
                y_n = y;
            break;
            
            case NORTH: /* gehe nach oben */
                x_n = x;
                y_n = y - 1;
            break;
            
            case SOUTH: /* gehe nach unten */
                x_n = x;
                y_n = y + 1;
            break;

            default:
                fprintf(stderr, "bug: unbekannte Richtung %d\n", dir);
                exit(EXIT_FAILURE);
        }

        if (is_empty(r, x_n, y_n)) {
            /* "entferne" Wand */
            switch (dir) {
                case EAST:
                    r->cube[y][x].east =
                    r->cube[y_n][x_n].west = 1;
                break;

                case WEST:
                    r->cube[y][x].west =
                    r->cube[y_n][x_n].east = 1;
                break;

                case NORTH:
                    r->cube[y][x].north =
                    r->cube[y_n][x_n].south = 1;
                break;

                case SOUTH:
                    r->cube[y][x].south =
                    r->cube[y_n][x_n].north = 1;
                break;
            }

            /* Oeffnungen haben sich geaendert -> neuen Typ ermitteln */
            r->cube[y][x].type     = get_type(&(r->cube[y][x]));
            r->cube[y_n][x_n].type = get_type(&(r->cube[y_n][x_n]));

            /* rekursiver Aufruf: naechstes Feld */
            random_fill_rectangle(r, x_n, y_n, count + 1);
        }

        dir++;
    }
}

/* erzeuge zufaelliges Labyrinth mit bestimmter Breite und Hoehe */
struct labyrinth *labyrinth_generate(int width, int height)
{
    struct labyrinth *l;
    struct rectangle *r;
    int x, y;

    l = labyrinth_new();
    r = rectangle_new(height, width, l->cube_cnt);

    random_fill_rectangle(r, 0, 0, 0);

    for (y = 0; y < r->height; y++)
        for (x = 0; x < r->width; x++)
            add_cube_by_type(l, get_type(&(r->cube[y][x])));

    l->solution = r;

    return l;
}

char is_solvable(struct labyrinth *l)
{
    int c, i, type, t;

    c = t = 0;
    type = l->cubes[0]->type;

    for (i = 0; i < l->cube_cnt; i++) {
        if (!t && type != l->cubes[i]->type)
            t = 1;

        c += l->cubes[i]->north + l->cubes[i]->east +
             l->cubes[i]->south + l->cubes[i]->west;
    }

    return c % 2 && t;
}
