/* 30. Bundeswettbewerb Informatik
 * Aufgabe 3 - Fehlerfrei puzzeln
 *
 * Autor: Matthias Rüster
 *        Lena Kristin Zander
 *
 * E-Mail: ruester@molgen.mpg.de
 *         zander@molgen.mpg.de
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <gd.h>
#include <math.h>
#include <limits.h>
#include <unistd.h>

#define BLUE   1
#define ORANGE 2
#define GREEN  3
#define YELLOW 4

#define RGB_BLACK  0,   0,   0
#define RGB_BLUE   39,  64,  139
#define RGB_YELLOW 255, 255, 0
#define RGB_ORANGE 255, 127, 0
#define RGB_GREEN  34,  139, 34

/* Standardwerte */
#define START_TEMP 0.22
#define STEPS      2e4
#define DECREASE   2e-5

struct stone {
    char up, down, left, right;
};

struct puzzle {
    struct stone **s;
    unsigned int height, width,
                 faults;
};

void usage(void)
{
    printf("Verwendung: aufgabe3 [Optionen] Dateiname\n\n"

           "moegliche Optionen:\n"
           "    -t Temperatur    Starttemperatur festlegen\n"
           "    -s Anzahl        Schrittanzahl festlegen\n"
           "    -d Wert          Verringerungswert festlegen\n"
           "    -h               diese Hilfe anzeigen\n");
}

/*
 * wandelt die Farben in Zahlen um
 */
char get_color(char *s)
{
    if (!strcasecmp(s, "blue"))
        return BLUE;

    if (!strcasecmp(s, "orange"))
        return ORANGE;

    if (!strcasecmp(s, "green"))
        return GREEN;

    if (!strcasecmp(s, "yellow"))
        return YELLOW;

    return 0;
}

/*
 * vergroessert das zweidimensionale stone-Array (wenn noetig)
 */
void resize(struct puzzle *p, unsigned int x, unsigned int y)
{
    unsigned int i;

    if (y > p->height) {
        if ((p->s = realloc(p->s, sizeof(*(p->s)) * y)) == NULL) {
            perror("resize: realloc");
            exit(EXIT_FAILURE);
        }

        for (i = p->height; i < y; i++) {
            if ((p->s[i] = calloc(p->width, sizeof(*(p->s[i])))) == NULL) {
                perror("resize: calloc");
                exit(EXIT_FAILURE);
            }
        }

        p->height = y;
    }

    if (x > p->width) {
        p->width = x;

        for (i = 0; i < p->height; i++) {
            if ((p->s[i] = realloc(p->s[i],
                                   sizeof(*(p->s[i])) * p->width)) == NULL) {
                perror("resize: realloc");
                exit(EXIT_FAILURE);
            }
        }
    }
}

struct puzzle *puzzle_new(unsigned int height, unsigned int width)
{
    struct puzzle *p;

    if ((p = calloc(1, sizeof(*p))) == NULL) {
        perror("puzzle_new: calloc");
        exit(EXIT_FAILURE);
    }

    resize(p, width, height);

    return p;
}

struct stone *stone_new(char up, char right, char down, char left)
{
    struct stone *s;

    if ((s = calloc(1, sizeof(*s))) == NULL) {
        perror("new_stone: calloc");
        exit(EXIT_FAILURE);
    }

    s->up    = up;
    s->right = right;
    s->down  = down;
    s->left  = left;

    return s;
}

/*
 * liest das Puzzle aus einer Datei ein
 */
struct puzzle *read_puzzle(char *filename)
{
    struct puzzle *p;
    char u, d, r, l;
    unsigned int x, y;
    FILE *f;
    char line[LINE_MAX], up[LINE_MAX],
         down[LINE_MAX], left[LINE_MAX],
         right[LINE_MAX];

    if ((f = fopen(filename, "r")) == NULL) {
        perror("init_puzzle: fopen");
        exit(EXIT_FAILURE);
    }

    if ((p = calloc(1, sizeof(*p))) == NULL) {
        perror("init_puzzle: calloc");
        exit(EXIT_FAILURE);
    }

    while (fgets(line, LINE_MAX, f) != NULL) {
        if (sscanf(line, "%d %d %s %s %s %s",
                   &x, &y, up, right, down, left) == EOF)
            continue;

        if (x <= 0 || y <= 0)
            continue;

        resize(p, x, y);

        u = get_color(up);
        r = get_color(right);
        d = get_color(down);
        l = get_color(left);

        if (!(u && d && l && r)) {
            fprintf(stderr, "unbekannte Farbe in Zeile \"%s\"\n", line);
            exit(EXIT_FAILURE);
        }

        p->s[y - 1][x - 1].up    = u;
        p->s[y - 1][x - 1].down  = d;
        p->s[y - 1][x - 1].right = r;
        p->s[y - 1][x - 1].left  = l;
    }

    if (!p->s) {
        fprintf(stderr, "Anfangsordnung konnte nicht eingelesen werden\n");
        exit(EXIT_FAILURE);
    }

    if (fclose(f)) {
        perror("init_puzzle: fclose");
        exit(EXIT_FAILURE);
    }

    return p;
}

void puzzle_free(struct puzzle *p)
{
    unsigned int i;

    for (i = 0; i < p->height; i++)
        free(p->s[i]);

    free(p->s);
    free(p);
}

/*
 * zaehlt die Fehler im Puzzle
 */
void count_faults(struct puzzle *p)
{
    unsigned int x, y;
    struct stone *s;

    p->faults = 0;

    for (x = 0; x < p->width; x++) {
        for (y = 0; y < p->height; y++) {
            s = &(p->s[y][x]);

            if (x < p->width  - 1
                && s->right != p->s[y][x + 1].left)
                p->faults++;

            if (y < p->height - 1
                && s->down  != p->s[y + 1][x].up)
                p->faults++;
        }
    }
}

/*
 * Funktionen zum Pruefen der Farbuebereinstimmung
 */
unsigned char check_up(struct puzzle *p, unsigned int x, unsigned int y)
{
    return (y > 0 && p->s[y][x].up != p->s[y - 1][x].down)
            ? 1 : 0;
}

unsigned char check_down(struct puzzle *p, unsigned int x, unsigned int y)
{
    return (y < p->height - 1 && p->s[y][x].down != p->s[y + 1][x].up)
            ? 1 : 0;
}

unsigned char check_left(struct puzzle *p, unsigned int x, unsigned int y)
{
    return (x > 0 && p->s[y][x].left != p->s[y][x - 1].right)
            ? 1 : 0;
}

unsigned char check_right(struct puzzle *p, unsigned int x, unsigned int y)
{
    return (x < p->width  - 1 && p->s[y][x].right != p->s[y][x + 1].left)
           ? 1 : 0;
}

unsigned int count_faults_2stones(struct puzzle *p,
                                  unsigned int x1, unsigned int y1,
                                  unsigned int x2, unsigned int y2)
{
    unsigned int c;

    c = 0;

    /* Stein 1 ist genau rechts von Stein 2 */
    if (x1 - 1 == x2 && y1 == y2)
        c += check_left(p, x1, y1);
    else
        c += check_left(p, x1, y1) + check_right(p, x2, y2);

    /* Stein 1 ist genau links von Stein 2 */
    if (x1 + 1 == x2 && y1 == y2)
        c += check_right(p, x1, y1);
    else
        c += check_right(p, x1, y1) + check_left(p, x2, y2);

    /* Stein 1 ist genau unterhalb von Stein 2 */
    if (y1 - 1 == y2 && x1 == x2)
        c += check_up(p, x1, y1);
    else
        c += check_up(p, x1, y1) + check_down(p, x2, y2);

    /* Stein 1 ist genau oberhalb von Stein 2 */
    if (y1 + 1 == y2 && x1 == x2)
        c += check_down(p, x1, y1);
    else
        c += check_down(p, x1, y1) + check_up(p, x2, y2);

    return c;
}

/*
 * speichert das Puzzle als png-Bild ab
 */
void puzzle2png(struct puzzle *p, char *filename)
{
    FILE *png;
    gdImagePtr image;
    char c;
    int black, blue, yellow, green, orange, color;
    unsigned int width, height,
                 edge, gap,
                 x, y, i, j, e;
    gdPoint point[5], triangle[3];

    edge   = 50;
    gap    = 2;
    width  = p->width  * edge + (p->width  - 1) * gap;
    height = p->height * edge + (p->height - 1) * gap;

    point[0].x = 0;
    point[0].y = 0;
    point[1].x = edge;
    point[1].y = 0;
    point[2].x = edge;
    point[2].y = edge;
    point[3].x = 0;
    point[3].y = edge;
    point[4].x = edge / 2;
    point[4].y = edge / 2;

    if ((png = fopen(filename, "w")) == NULL) {
        perror("puzzle_save: fopen");
        exit(EXIT_FAILURE);
    }

    if ((image = gdImageCreate(width + 1, height + 1)) == NULL) {
        perror("puzzle_save: gdImageCreate");
        exit(EXIT_FAILURE);
    }

    black  = gdImageColorAllocate(image, RGB_BLACK);
    blue   = gdImageColorAllocate(image, RGB_BLUE);
    yellow = gdImageColorAllocate(image, RGB_YELLOW);
    green  = gdImageColorAllocate(image, RGB_GREEN);
    orange = gdImageColorAllocate(image, RGB_ORANGE);

    gdImageFill(image, 0, 0, green);

    /* draw vertical lines of grid */
    for (x = edge; x < width; x += edge + gap)
        for (i = 0; i < gap; i++)
            gdImageLine(image, x + i, 0, x + i, height, black);

    /* draw vertical lines of grid */
    for (y = edge; y < height; y += edge + gap)
        for (i = 0; i < gap; i++)
            gdImageLine(image, 0, y + i, width, y + i, black);

    for (x = i = 0; x < width; x += edge + gap, i++) {
        for (y = j = 0; y < height; y += edge + gap, j++) {
            triangle[2].x = x + point[4].x;
            triangle[2].y = y + point[4].y;

            for (e = 0; e < 4; e++) {
                triangle[0].x = x + ((e == 0 || e == 3)
                                     ? point[0].x : point[2].x);
                triangle[0].y = y + ((e == 0 || e == 3)
                                     ? point[0].y : point[2].y);
                triangle[1].x = x + ((e == 0 || e == 1)
                                     ? point[1].x : point[3].x);
                triangle[1].y = y + ((e == 0 || e == 1)
                                     ? point[1].y : point[3].y);

                switch (e) {
                    case 0:
                        c = p->s[j][i].up;
                        break;

                    case 1:
                        c = p->s[j][i].right;
                        break;

                    case 2:
                        c = p->s[j][i].down;
                        break;

                    case 3:
                        c = p->s[j][i].left;
                        break;
                }

                color = black;

                switch (c) {
                    case BLUE:
                        color = blue;
                        break;

                    case YELLOW:
                        color = yellow;
                        break;

                    case GREEN:
                        color = green;
                        break;

                    case ORANGE:
                        color = orange;
                        break;
                }

                gdImageFilledPolygon(image, triangle, 3, color);
            }
        }
    }

    gdImagePng(image, png);
    fclose(png);
    gdImageDestroy(image);
}

void stone_swap(struct stone *a, struct stone *b)
{
    struct stone c;

    memcpy(&c, a, sizeof(*a));
    memcpy(a,  b, sizeof(*a));
    memcpy(b, &c, sizeof(*a));
}

void copy_stones(struct puzzle *dest, struct puzzle *src)
{
    unsigned int i;

    for (i = 0; i < src->height; i++)
        memcpy(dest->s[i], src->s[i], sizeof(*(dest->s[0])) * src->width);

    dest->faults = src->faults;
}

/*
 * Algorithmus zum Finden einer optimalen Loesung
 */
void simulated_annealing(struct puzzle *p,
                         double starttemp, double steps, double decrease)
{
    double t, r, pr;
    unsigned int n, x1, y1, x2, y2, c, f;
    char tty;
    struct puzzle *min;

    tty         = isatty(STDOUT_FILENO) ? 1 : 0;
    min         = puzzle_new(p->height, p->width);
    min->faults = INT_MAX;
    count_faults(p);

    for (t = starttemp; t > 0.0 && p->faults != 0; t -= decrease) {
        for (n = 0; n < steps && p->faults != 0; n++) {
            /* 2 Steine zufaellig waehlen */
            x1 = rand() % p->width;
            y1 = rand() % p->height;

            do {
                x2 = rand() % p->width;
                y2 = rand() % p->height;
            } while (x1 == x2 && y1 == y2);

            f = count_faults_2stones(p, x1, y1, x2, y2);

            stone_swap(&p->s[y1][x1], &p->s[y2][x2]);

            /* neuen Fehlerwert berechnen */
            c = p->faults + (count_faults_2stones(p, x1, y1, x2, y2) - f);

            /* zufaellige Zahl zwischen 0 und 1 */
            r = (double) rand() / (double) RAND_MAX;

            /* Wahrscheinlichkeit dafuer, dass
             * das schlechteres Puzzle behalten wird */
            pr = exp(-(((double) c - (double) p->faults) / t));

            if (c > p->faults && r > pr)
                /* schlechteres Puzzle wird verworfen */
                stone_swap(&p->s[y2][x2], &p->s[y1][x1]);
            else
                p->faults = c;

            if (min->faults > p->faults)
                copy_stones(min, p);
        }

        if (tty) {
            printf("\rTemperatur: %.3f    Strafpunkte: % 4d", t, p->faults);
            fflush(stdout);
        }
    }

    if (tty)
        printf("\r");

    copy_stones(p, min);

    if (p->faults)
        printf("Das bestes Puzzle hat %d Strafpunkte.    \n", p->faults);
    else
        printf("Es wurde ein optimales Ergebnis gefunden.\n");

    puzzle_free(min);
}

int main(int argc, char *argv[])
{
    struct puzzle *p;
    double starttemp, steps, decrease;
    int opt;
    char outfile[LINE_MAX];

    if (argc < 2) {
        fprintf(stderr,
                "Datei mit der Anfangsordnung als Parameter angeben\n");
        return EXIT_FAILURE;
    }

    srand(time(NULL) + getpid());

    starttemp = START_TEMP;
    steps     = STEPS;
    decrease  = DECREASE;

    while ((opt = getopt(argc, argv, "ht:s:d:")) != -1) {
        switch (opt) {
            case 'h':
                usage();
                return EXIT_FAILURE;

            case 't':
                if ((starttemp = atof(optarg)) <= 0.0) {
                    fprintf(stderr,
                            "Starttemperatur muss groesser als 0 sein\n");
                    return EXIT_FAILURE;
                }
                break;

            case 's':
                if ((steps = atof(optarg)) < 1.0) {
                    fprintf(stderr,
                            "Schrittanzahl muss groesser als 1 sein\n");
                    return EXIT_FAILURE;
                }
                break;

            case 'd':
                if ((decrease = atof(optarg)) <= 0.0) {
                    fprintf(stderr,
                            "Verringerungswert muss groesser als 0 sein\n");
                    return EXIT_FAILURE;
                }
                break;

            default:
                if (optopt == 't' || optopt == 's' || optopt == 'd')
                    fprintf(stderr, "Option -%c benoetigt ein Argument\n",
                            optopt);
                else
                    fprintf(stderr, "unbekannte Option -%c\n", optopt);

                usage();
                return EXIT_FAILURE;
        }
    }

    if (argc - optind > 1) {
        fprintf(stderr, "Zu viele Parameter\n");
        return EXIT_FAILURE;
    }

    if (argc == optind) {
        fprintf(stderr,
                "Datei mit der Anfangsordnung als Parameter angeben\n");
        return EXIT_FAILURE;
    }

    /* Puzzle einlesen */
    p = read_puzzle(argv[argc - 1]);

    /* Puzzle loesen */
    simulated_annealing(p, starttemp, steps, decrease);

    /* Puzzle abspeichern */
    sprintf(outfile, "%d.png", p->faults);
    puzzle2png(p, outfile);
    printf("Das Ergebnis wurde in der Datei %s gespeichert.\n", outfile);

    puzzle_free(p);

    return EXIT_SUCCESS;
}
