/* 30. Bundeswettbewerb Informatik
 * Aufgabe 2 - Aladins Lampen
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
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>
#include <math.h>
#include <sys/sysinfo.h>

#define RANDOM -1

#define MIN(a, b) ((a) < (b) ? (a) : (b))

struct game {
    uint64_t *button; /* Array von Bitmasken */
    uint64_t lamps;   /* Status der Lampen */
    int count;
};

void usage(void)
{
    printf("Verwendung: aufgabe2 [Optionen] [Dateiname]\n\n"

           "Einlesen einer bestimmten Schaltung:\n"
           "    -f Dateiname      Schaltung auf Brauchbarkeit pruefen\n"
           "    -s Dateiname      Tastenfolge zum Loesen "
                                 "der Schaltung ermitteln\n\n"

           "Beim Generieren der zufaelligen Schaltungen\n"
           "koennen folgende Parameter festgelegt werden:\n"
           "    -l Lampen         Anzahl der Lampen festlegen\n"
           "    -c Verbindungen   Anzahl der Verbindungen festlegen\n");
}

/*
 * liefert die groesst moegliche Anzahl an Lampen
 * (abhaengig von der RAM-Groesse)
 */
int get_max(void)
{
    struct sysinfo s;

    if (sysinfo(&s)) {
        fprintf(stderr, "RAM-Groesse konnte nicht ermittelt werden\n");
        s.totalram = 1 << 30; /* ungefaehr 1 GB */
    }

    return MIN(log(8.0 * s.totalram) / M_LN2, 63);
}

/*
 * ueberprueft die Anzahl der Lampen
 */
char check_count(int count)
{
    return count > 1 && count <= get_max();
}

/*
 * alloziert Speicher fuer eine game-Struktur
 * und initialisiert die Bitmasken
 */
struct game *game_new(int count)
{
    int i;
    struct game *g;

    if ((g = calloc(1, sizeof(*g))) == NULL
        || (g->button = calloc(count, sizeof(*(g->button)))) == NULL) {
        perror("init_game: calloc");
        exit(EXIT_FAILURE);
    }

    g->count = count;

    /* Bitmasken initialisieren */
    for (i = 0; i < g->count; i++)
        g->button[i] |= ((uint64_t) 1 << i);

    return g;
}

/*
 * liest eine Schaltung aus einer Datei ein
 */
struct game *read_initial_situation(char *filename)
{
    FILE *f;
    char line[LINE_MAX];
    char *token;
    char *delims = " \t";
    int from, to, i, c;
    struct game *g;

    if ((f = fopen(filename, "r")) == NULL) {
        perror("read_initial_situation: fopen");
        exit(EXIT_FAILURE);
    }

    /* Anzahl der Lampen einlesen */
    if (fgets(line, LINE_MAX, f) == NULL
        || sscanf(line, "%d", &c) == EOF) {
        fprintf(stderr, "Anzahl der Taster konnte nicht eingelesen werden\n");
        exit(EXIT_FAILURE);
    }

    /* Anzahl der Lampen ueberpruefen */
    if (!check_count(c)) {
        fprintf(stderr, "Falsche Anzahl an Tastern (%d)\n", c);
        exit(EXIT_FAILURE);
    }

    g = game_new(c);
    i = 1;

    while (fgets(line, LINE_MAX, f) != NULL) {
        i++;

        /* '\n' am Ende entfernen */
        line[strlen(line) - 1] = '\0';

        /* Tasternummer einlesen */
        token = strtok(line, delims);
        from  = atoi(token);

        if (from <= 0 || from > g->count) {
            fprintf(stderr, "Falsche Tasternummer '%s' (Zeile %d)\n",
                    token, i);
            exit(EXIT_FAILURE);
        }

        /* Verbindungen einlesen */
        while ((token = strtok(NULL, delims)) != NULL) {
            if (!strcasecmp(token, "an")) {
                g->lamps |= ((uint64_t) 1) << (from - 1);
                continue;
            }

            if (!strcasecmp(token, "aus")) {
                g->lamps &= ~((uint64_t) 1 << (from - 1));
                continue;
            }

            to = atoi(token);

            if (to <= 0 || to > g->count) {
                fprintf(stderr, "Falsche Lampennummer '%s' (Zeile %d)\n",
                        token, i);
                exit(EXIT_FAILURE);
            }

            /* Verbindung eintragen */
            g->button[from - 1] |= ((uint64_t) 1 << (to - 1));
        }
    }

    if (fclose(f)) {
        perror("read_initial_situation: fclose");
        exit(EXIT_FAILURE);
    }

    return g;
}

void game_print(struct game *g)
{
    int i, j;

    printf("Lampenanzahl: %d\nSchaltung:\n", g->count);

    for (i = 0; i < g->count; i++) {
        printf("%2d:", i + 1);

        /* Verbindungen des Tasters ausgeben */
        for (j = 0; j < g->count; j++)
            if (g->button[i] & ((uint64_t) 1 << j))
                printf(" %d", j + 1);

        printf("\n");
    }

    printf("\n");
}

void game_print_state(struct game *g)
{
    int i, j;

    printf("Lampenanzahl: %d\nSchaltung:\n", g->count);

    for (i = 0; i < g->count; i++) {
        printf("%2d (%s):", i + 1,
               (((uint64_t) 1 << i) & g->lamps) ? " an" : "aus");

        /* Verbindungen des Tasters ausgeben */
        for (j = 0; j < g->count; j++)
            if (g->button[i] & ((uint64_t) 1 << j))
                printf(" %d", j + 1);

        printf("\n");
    }

    printf("\n");
}

void game_free(struct game *g)
{
    free(g->button);
    free(g);
}

void push_button(struct game *g, int p)
{
    g->lamps ^= g->button[p];
}

/*
 * rekursive Funktion zum Suchen aller Ausgangssituationen
 */
void check(struct game *g, unsigned char *m)
{
    int i;
    char s;

    for (i = 0; i < g->count; i++) {
        push_button(g, i);

        /* die letzten 3 Bits von g->lamps */
        s = 7 & g->lamps;

        /* pruefen, ob Ausgangssituation schon einmal vorkam */
        if (m[g->lamps >> 3] & (1 << s)) {
            push_button(g, i);
            return;
        }

        /* erreichte Ausgangssituation merken */
        m[g->lamps >> 3] |= (1 << s);

        check(g, m);

        push_button(g, i);
    }
}

char game_check(struct game *g)
{
    uint64_t i, s;
    unsigned char *m;
    char r;

    r = 1;

    /* Anzahl der benoetigten char-Variablen */
    s = ((uint64_t) 1 << g->count) >> 3;

    if ((m = calloc(s, 1)) == NULL) {
        perror("game_check: calloc");
        exit(EXIT_FAILURE);
    }

    /* alle Lampen "anschalten" */
    g->lamps = (((uint64_t) 1 << g->count) - 1);

    check(g, m);

    if (g->count == 2) {
        r = (m[0] == 0xF);
        free(m);
        return r;
    }

    /* pruefen, ob alle moeglichen
     * Ausgangssituationen errreicht wurden */
    for (i = 0; i < (((uint64_t) 1 << g->count) >> 3); i++) {
        if (m[i] != 0xFF) {
            r = 0;
            break;
        }
    }

    free(m);
    return r;
}

char is_solved(struct game *g)
{
    return g->lamps == (((uint64_t) 1 << g->count) - 1);
}

char solve(struct game *g, unsigned char *m,
           uint64_t initial, int *solution, int depth)
{
    int i;
    char s;

    for (i = 0; i < g->count; i++) {
        push_button(g, i);

        /* die letzten 3 Bits von g->lamps */
        s = 7 & g->lamps;

        /* pruefen, ob Ausgangssituation schon einmal vorkam */
        if (m[g->lamps >> 3] & (1 << s)) {
            push_button(g, i);
            return 0;
        }

        solution[depth] = i;

        if (g->lamps == initial)
            return 1;

        /* erreichte Ausgangssituation merken */
        m[g->lamps >> 3] |= (1 << s);

        if (solve(g, m, initial, solution, depth + 1))
            return 1;

        solution[depth] = -1;

        push_button(g, i);
    }

    return 0;
}

char game_solve(struct game *g)
{
    int i;
    uint64_t initial, s;
    unsigned char *m;
    int *solution;

    if (is_solved(g)) {
        printf("Alle Lampen sind bereits angeschaltet\n");
        return 1;
    }

    if ((solution = calloc(g->count, sizeof(*solution))) == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    memset(solution, -1, g->count * sizeof(*solution));

    /* Anzahl der benoetigten char-Variablen */
    s = ((uint64_t) 1 << g->count) >> 3;

    if ((m = calloc(s, 1)) == NULL) {
        perror("game_solve: calloc");
        exit(EXIT_FAILURE);
    }

    initial = g->lamps;

    /* alle Lampen "anschalten" */
    g->lamps = (((uint64_t) 1 << g->count) - 1);

    solve(g, m, initial, solution, 0);

    if (solution[0] == -1) {
        free(solution);
        free(m);
        return 0;
    }

    printf("Die Schaltung kann mit folgender Tastenfolge geloest werden:\n");

    for (i = 0; i < g->count && solution[i] != -1; i++)
        printf("%d ", solution[i] + 1);
    printf("\n");

    free(solution);
    free(m);

    return 1;
}

/*
 * erzeugt eine zufaellige Schaltung
 */
struct game *create_random(int lamp_cnt, int con_cnt)
{
    struct game *g;
    int from, to, i;

    if (lamp_cnt == RANDOM)
        lamp_cnt = 2 + rand() % (get_max() - 2);

    g = game_new(lamp_cnt);

    if (con_cnt == RANDOM)
        con_cnt = rand() % (g->count * g->count - g->count + 1);
    else
        con_cnt = MIN(con_cnt, g->count * g->count - g->count);

    /* zufaellige Verbindungen erzeugen */
    for (i = 0; i < con_cnt; i++) {
        do {
            from = rand() % g->count;
            to   = rand() % g->count;
        } while (to == from || (g->button[from] & ((uint64_t) 1 << to)));

        g->button[from] |= ((uint64_t) 1 << to);
    }

    return g;
}

int main(int argc, char *argv[])
{
    struct game *g;
    char *filename, *solve;
    int opt, lamp_cnt, con_cnt;

    lamp_cnt = con_cnt = RANDOM;
    filename = solve   = NULL;
    g        = NULL;

    /* Parameter einlesen */
    while ((opt = getopt(argc, argv, "hl:c:f:s:")) != -1) {
        switch (opt) {
            case 'h':
                usage();
                return EXIT_SUCCESS;

            case 'f':
                filename = optarg;
                break;

            case 's':
                solve = optarg;
                break;

            case 'l':
                lamp_cnt = atoi(optarg);

                if (!check_count(lamp_cnt)) {
                    if (lamp_cnt <= 1)
                        fprintf(stderr,
                        "Die Anzahl der Lampen muss groesser als 1 sein\n");
                    else
                        fprintf(stderr,
                        "Die Anzahl der Lampen ist zu gross\n");

                    return EXIT_FAILURE;
                }

                break;

            case 'c':
                con_cnt = atoi(optarg);

                if (con_cnt < 0) {
                    fprintf(stderr,
                    "Die Anzahl der Verbindungen kann nicht negativ sein\n");

                    return EXIT_FAILURE;
                }

                break;

            case '?':
                if (optopt == 'l' || optopt == 'c' || optopt == 'f')
                    fprintf(stderr, "Option -%c benoetigt ein Argument\n",
                            optopt);
                else
                    fprintf(stderr, "unbekannte Option -%c\n", optopt);

                usage();
                return EXIT_FAILURE;
        }
    }

    srand(time(NULL) + getpid());

    if (solve) {
        g = read_initial_situation(solve);
        game_print_state(g);

        if (!game_solve(g))
            printf("Es konnte keine Tastenfolge gefunden werden.\n");
    } else if (filename) {
        /* Schaltung aus der Datei einlesen */
        g = read_initial_situation(filename);
        game_print(g);

        if (game_check(g))
            printf("Die Schaltung ist brauchbar.\n");
        else
            printf("Die Schaltung ist nicht brauchbar.\n");
    } else {
        /* zufaellige Schaltungen erzeugen,
         * bis eine brauchbare gefunden wurde */
        do {
            if (g)
                game_free(g);

            g = create_random(lamp_cnt, con_cnt);
        } while (!game_check(g));

        printf("Brauchbare Schaltung gefunden:\n");
        game_print(g);
    }

    game_free(g);

    return EXIT_SUCCESS;
}
