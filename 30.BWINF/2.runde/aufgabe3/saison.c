/* 30. Bundeswettbewerb Informatik - 2. Runde
 * Aufgabe 3 - Trickey
 *
 * Autor: Matthias Rüster
 *
 * E-Mail: ruester@molgen.mpg.de
 */

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <gmp.h>
#include <ctype.h>

#include "saison.h"
#include "team.h"

#define UNKNOWN -1

#define SWAP(a, b, typ) {typ h = a; a = b; b = h;}
#define MAX(a, b)       (((a) > b) ? (a) : (b))

static unsigned long int binom(int n, int k)
{
    unsigned long b;

    mpz_t n_fac, k_fac, nk_fac;

    mpz_init(n_fac);
    mpz_init(k_fac);
    mpz_init(nk_fac);

    mpz_fac_ui(n_fac, n);
    mpz_fac_ui(k_fac, k);
    mpz_fac_ui(nk_fac, n - k);

    mpz_mul(nk_fac, nk_fac, k_fac);
    mpz_tdiv_q(n_fac, n_fac, nk_fac);

    b = mpz_get_ui(n_fac);

    mpz_clear(n_fac);
    mpz_clear(k_fac);
    mpz_clear(nk_fac);

    return b;
}

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

static struct saison *saison_new(void)
{
    struct saison *s;

    if ((s = calloc(1, sizeof(*s))) == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    return s;
}

static int get_team_id(struct saison *s, char *teamname)
{
    int i;

    for (i = 0; i < s->team_cnt; i++)
        if (!strcmp(s->teams[i]->name, teamname))
            return i;

    return -1;
}

static void add_team(struct saison *s, char *teamname)
{
    if (get_team_id(s, teamname) != -1) {
        fprintf(stderr, "Mannschaft '%s' ist mehrfach definiert worden\n",
                teamname);
        exit(EXIT_FAILURE);
    }

    s->teams = realloc(s->teams, sizeof(*(s->teams)) * (s->team_cnt + 1));

    if (!s->teams) {
        perror("realloc");
        exit(EXIT_FAILURE);
    }

    s->ranking = realloc(s->ranking,
                         sizeof(*(s->ranking)) * (s->team_cnt + 1));

    if (!s->ranking) {
        perror("realloc");
        exit(EXIT_FAILURE);
    }

    s->teams[s->team_cnt]   = team_new(teamname);
    s->ranking[s->team_cnt] = s->teams[s->team_cnt];
    s->team_cnt++;
}

static struct team *get_team(struct saison *s, char *teamname)
{
    int i;

    for (i = 0; i < s->team_cnt; i++)
        if (!strcmp(s->teams[i]->name, teamname))
            return s->teams[i];

    return NULL;
}

static char has_colon(char *s)
{
    while (*s) {
        if (*s == ':')
            return 1;

        s++;
    }

    return 0;
}

static void sort_by_team_id(int *game, int *goals, int cnt)
{
    int i, j, m, n;

    for (i = 1; i < cnt; i++) {
        m = goals[i];
        n = game[i];

        for (j = i; j > 0; j--) {
            if (game[j - 1] < n)
                break;

            goals[j] = goals[j - 1];
            game[j]  = game[j - 1];
        }

        goals[j] = m;
        game[j]  = n;
    }
}

static void sort_by_goals(int *game, int *goals, int cnt)
{
    int i, j, m, n;

    for (i = 1; i < cnt; i++) {
        m = goals[i];
        n = game[i];

        for (j = i; j > 0; j--) {
            if (goals[j - 1] >= m)
                break;

            goals[j] = goals[j - 1];
            game[j]  = game[j - 1];
        }

        goals[j] = m;
        game[j]  = n;
    }
}

static void init_games(struct saison *s, int *temp, int pos)
{
    if (pos == s->teams_per_game) {
        memcpy(s->games[s->game_cnt], temp,
               s->teams_per_game * sizeof(*temp));
        s->game_cnt++;
        return;
    }

    if (pos == 0)
        temp[pos] = 0;
    else
        temp[pos] = temp[pos - 1] + 1;

    while (temp[pos] < s->team_cnt) {
        init_games(s, temp, pos + 1);
        temp[pos]++;
    }
}

static void print_game(struct saison *s, int index, FILE *f)
{
    int i;

    for (i = 0; i < s->teams_per_game; i++)
        fprintf(f, "%s%c", s->teams[s->games[index][i]]->name,
               (i == s->teams_per_game - 1) ? '\0' : ':');
}

/* struct saison initialisieren: alle moeglichen Spiele hinzufuegen */
static void init_saison(struct saison *s)
{
    int b, i;
    int *temp;

    b = binom(s->team_cnt, s->teams_per_game);

    if ((s->games = calloc(b, sizeof(*(s->games)))) == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < b; i++) {
        s->games[i] = calloc(s->teams_per_game, sizeof(*(s->games[i])));

        if (s->games[i] == NULL) {
            perror("calloc");
            exit(EXIT_FAILURE);
        }
    }

    if ((temp = calloc(s->teams_per_game, sizeof(*temp))) == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    s->game_cnt = 0;
    init_games(s, temp, 0);

    free(temp);

    if ((s->goals = calloc(b, sizeof(*(s->goals)))) == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < b; i++) {
        s->goals[i] = calloc(s->teams_per_game, sizeof(*(s->goals[i])));

        if (s->goals[i] == NULL) {
            perror("calloc");
            exit(EXIT_FAILURE);
        }

        memset(s->goals[i], UNKNOWN,
               s->teams_per_game * sizeof(*(s->goals[i])));
    }
}

static int add_result(struct saison *s, int *game, int *goals)
{
    int i;

    sort_by_team_id(game, goals, s->teams_per_game);

    for (i = 0; i < s->game_cnt; i++) {
        if (!memcmp(s->games[i], game, sizeof(*game) * s->teams_per_game)) {
            if (s->goals[i][0] != UNKNOWN) {
                fprintf(stderr, "Spielergebnis fuer ");
                print_game(s, i, stderr);
                fprintf(stderr, " doppelt definiert\n");
                exit(EXIT_FAILURE);
            }

            memcpy(s->goals[i], goals, sizeof(*goals) * s->teams_per_game);
            return 1;
        }
    }

    return 0;
}

void saison_print(struct saison *s)
{
    int i, j;

    for (i = 0; i < s->game_cnt; i++) {
        sort_by_team_id(s->games[i], s->goals[i], s->teams_per_game);

        for (j = 0; j < s->teams_per_game; j++)
            printf("%s%s", s->teams[s->games[i][j]]->name,
                   (j == s->teams_per_game - 1) ? " - " : ":");

        if (s->goals[i][0] == UNKNOWN) {
            printf("nicht bekannt\n");
            continue;
        }

        for (j = 0; j < s->teams_per_game; j++)
            printf("%d%c", s->goals[i][j],
                   (j == s->teams_per_game - 1) ? '\n' : ':');
    }

    puts("\nRangliste:");

    for (i = 0; i < s->team_cnt; i++)
        printf("%d. %s\t(%-3d Punkte | %-3d Tore)\n",
               s->ranking[i]->rank, s->ranking[i]->name,
               s->ranking[i]->points, s->ranking[i]->goals);

    puts("");
}

/* Rangliste der Mannschaften aktualisieren */
static void update_ranking(struct saison *s)
{
    int i, j;
    struct team *p;

    /* Insertionsort - Mannschaften nach Punkten sortieren */
    for (i = 1; i < s->team_cnt; i++) {
        p = s->ranking[i];

        for (j = i; j > 0; j--) {
            if (s->ranking[j - 1]->points > p->points)
                break;

            if (s->ranking[j - 1]->points == p->points
                && s->ranking[j - 1]->goals >= p->goals)
                break;

            s->ranking[j] = s->ranking[j - 1];
        }

        s->ranking[j] = p;
    }

    /* Plaetze herausfinden */
    s->ranking[0]->rank = 1;

    for (i = 1; i < s->team_cnt; i++) {
        s->ranking[i]->rank = i + 1;

        if (s->ranking[i]->points == s->ranking[i - 1]->points
            && s->ranking[i]->goals == s->ranking[i - 1]->goals)
            s->ranking[i]->rank = s->ranking[i - 1]->rank;
    }
}

/* Auswerten eines Spieles */
static void game_evaluation(struct saison *s, int i)
{
    int points;
    int j, equal_cnt;

    if (s->goals[i][0] == UNKNOWN)
        return;

    sort_by_goals(s->games[i], s->goals[i], s->teams_per_game);

    /* geschossene Tore bei den Teams hinzuaddieren */
    for (j = 0; j < s->teams_per_game; j++)
        s->teams[s->games[i][j]]->goals += s->goals[i][j];

    /* nur ein Gewinner? */
    if (s->goals[i][0] != s->goals[i][1]) {
        /* Punkte vergeben */
        points = (1 << (s->teams_per_game - 1)) + 1;
        s->teams[s->games[i][0]]->points += points;
        return;
    }

    /* es gibt mindestens 2 Mannschaften mit gleicher Toranzahl */
    equal_cnt = 2;

    /* wie viele Mannschaften haben die gleiche Anzahl an Toren? */
    for (j = 1; j < s->teams_per_game - 1; j++) {
        if (s->goals[i][j + 1] == s->goals[i][j])
            equal_cnt++;
        else
            break;
    }

    /* zu vergebene Punkte richten sich nach
       Anzahl der Mannschaften mit der gleichen Anzahl an Toren */
    points = (1 << (s->teams_per_game - equal_cnt));

    /* Punkte vergeben */
    for (j = 0; j < equal_cnt; j++)
        s->teams[s->games[i][j]]->points += points;
}

/* jedes Spiel auswerten */
static void saison_evaluation(struct saison *s)
{
    int i;

    for (i = 0; i < s->game_cnt; i++)
        game_evaluation(s, i);

    update_ranking(s);
}

/* Saison aus Datei laden */
struct saison *saison_load(char *fn)
{
    FILE *f;
    char line[LINE_MAX];
    struct saison *s;
    char delim_t[] = ":",
         delim_g[] = "-";
    char *team, *result, *token;
    int line_cnt, length,
        team_cnt, goal_cnt,
        cnt;
    int *game, *goals;

    if ((f = fopen(fn, "r")) == NULL) {
        fprintf(stderr, "Datei '%s' konnte nicht geoeffnet werden: ", fn);
        perror("");
        exit(EXIT_FAILURE);
    }

    s        = saison_new();
    line_cnt = 1;
    game     = goals = NULL;

    /* Anzahl der Mannschaften einlesen */
    if (!fgets(line, LINE_MAX, f)) {
        fprintf(stderr,
                "%s: Anzahl der Mannschaften konnte "
                "nicht eingelesen werden\n", fn);
        exit(EXIT_FAILURE);
    }

    if ((cnt = atoi(line)) < 2) {
        fprintf(stderr,
                "%s: Anzahl der Mannschaften muss groesser als 1 sein\n", fn);
        exit(EXIT_FAILURE);
    }

    /* alle Mannschaftsnamen einlesen */
    while (fgets(line, LINE_MAX, f)) {
        line_cnt++;
        length = strlen(line);

        /* eventuelle whitespaces entfernen */
        while (length > 0 && isspace(line[length - 1])) {
            line[length - 1] = '\0';
            length--;
        }

        if (has_colon(line))
            break;

        if (!get_team(s, line)) {
            add_team(s, line);
        } else {
            s->query = strdup(line);
            fgets(line, LINE_MAX, f);
            line_cnt++;
            break;
        }
    }

    if (!s->query) {
        fprintf(stderr, "%s: keine zu untersuchende Mannschaft angegeben\n",
                fn);
        exit(EXIT_FAILURE);
    }

    if (s->team_cnt < 2) {
        fprintf(stderr,
                "%s: Anzahl der Mannschaften muss groesser als 1 sein\n", fn);
        exit(EXIT_FAILURE);
    }

    if (cnt != s->team_cnt) {
        fprintf(stderr,
                "%s: Anzahl der eingelesenen Mannschaften "
                "stimmen nicht mit festgelegtem Wert ueberein\n", fn);
        exit(EXIT_FAILURE);
    }

    if (!has_colon(line)) {
        fprintf(stderr, "%s: Zeile %d: keine Spielergebnisse angegeben\n",
                fn, line_cnt);
        exit(EXIT_FAILURE);
    }

    /* alle Spielergebnisse einlesen */
    do {
        length = strlen(line);

        if (length == 1) {
            line_cnt++;
            continue;
        }

        /* eventuelle whitespaces entfernen */
        while (length > 0 && isspace(line[length - 1])) {
            line[length - 1] = '\0';
            length--;
        }

        /* bei '-' teilen */
        if (!(team = strtok(line, delim_g))
            || !(result = strtok(NULL, delim_g))
            || strtok(NULL, delim_g) != NULL) {
            fprintf(stderr, "%s: Zeile %d konnte nicht eingelesen werden\n",
                    fn, line_cnt);
            exit(EXIT_FAILURE);
        }

        team     = trim(team);
        result   = trim(result);
        team_cnt = goal_cnt = 0;

        /* Mannschaften bei ':' teilen */
        if (!(token = strtok(team, delim_t))) {
            fprintf(stderr, "%s: Zeile %d konnte nicht eingelesen werden\n",
                    fn, line_cnt);
            exit(EXIT_FAILURE);
        }

        do {
            team_cnt++;

            if ((game = realloc(game, sizeof(*game) * team_cnt)) == NULL) {
                perror("saison_load: realloc");
                exit(EXIT_FAILURE);
            }

            token = trim(token);

            game[team_cnt - 1] = get_team_id(s, token);

            if (game[team_cnt - 1] == -1) {
                fprintf(stderr, "%s: Zeile %d: unbekanntes Team '%s'\n",
                        fn, line_cnt, token);
                exit(EXIT_FAILURE);
            }
        } while ((token = strtok(NULL, delim_t)));

        /* Tore bei ':' teilen */
        if (!(token = strtok(result, delim_t))) {
            fprintf(stderr, "%s: Zeile %d konnte nicht eingelesen werden\n",
                    fn, line_cnt);
            exit(EXIT_FAILURE);
        }

        do {
            goal_cnt++;

            goals = realloc(goals, sizeof(*goals) * goal_cnt);

            if (goals == NULL) {
                perror("realloc");
                exit(EXIT_FAILURE);
            }

            /* Tore einlesen */
            goals[goal_cnt - 1] = atoi(token);

            if (goals[goal_cnt - 1] < 0) {
                fprintf(stderr,
                        "%s: Zeile %d: falscher Wert '%s'\n",
                        fn, line_cnt, token);
                exit(EXIT_FAILURE);
            }
        } while ((token = strtok(NULL, delim_t)));

        /* Zaehlvariablen ueberpruefen */
        if (team_cnt != goal_cnt) {
            fprintf(stderr,
                    "%s: Fehler in Zeile %d: Anzahl der Mannschaften "
                    "ist ungleich der Anzahl der Ergebnisse\n",
                    fn, line_cnt);
            exit(EXIT_FAILURE);
        }

        if (team_cnt < 2) {
            fprintf(stderr,
                    "%s: Zeile %d: es muss mindestens zwei "
                    "Mannschaften pro Spiel geben\n",
                    fn, line_cnt);
        }

        if (!s->teams_per_game) {
            s->teams_per_game = team_cnt;
            init_saison(s);
        }

        if (s->teams_per_game != team_cnt) {
            fprintf(stderr,
                    "%s: Zeile %d: Anzahl der Mannschaften "
                    "pro Spiel hat sich geaendert\n",
                    fn, line_cnt);
            exit(EXIT_FAILURE);
        }

        /* Spielergebnis eintragen */
        if (!add_result(s, game, goals)) {
            fprintf(stderr,
                    "%s: Zeile %d: ungueltige Kombination der Mannschaften\n",
                    fn, line_cnt);
            exit(EXIT_FAILURE);
        }

        line_cnt++;
    } while (fgets(line, LINE_MAX, f));

    free(goals);
    free(game);

    if (fclose(f) == EOF) {
        fprintf(stderr, "Datei '%s' konnte nicht geschlossen werden: ", fn);
        perror("");
        exit(EXIT_FAILURE);
    }

    saison_evaluation(s);

    return s;
}

void saison_free(struct saison *s)
{
    int i;

    for (i = 0; i < s->game_cnt; i++) {
        free(s->games[i]);
        free(s->goals[i]);
    }

    free(s->games);
    free(s->goals);

    for (i = 0; i < s->team_cnt; i++)
        team_free(s->teams[i]);

    free(s->teams);
    free(s->ranking);
    free(s->query);
    free(s);
}

int calculate_best_rank(struct saison *s, char *teamname)
{
    char in_game;
    char *let_lose;
    int i, j;
    struct team *self, *opponent;

    /* self = zu untersuchende Mannschaft */
    if (!(self = get_team(s, teamname))) {
        fprintf(stderr, "Mannschaft '%s' unbekannt\n", teamname);
        return -1;
    }

    if ((let_lose = calloc(s->teams_per_game, sizeof(*let_lose))) == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < s->game_cnt; i++) {
        if (s->goals[i][0] != UNKNOWN)
            continue;

        in_game = 0;

        for (j = 0; j < s->teams_per_game; j++) {
            opponent = s->teams[s->games[i][j]];

            if (opponent == self) {
                /* self ist im Spiel */
                in_game = 1;
                break;
            }

            /* "bessere" Mannschaften verlieren lassen */
            if (opponent->rank <= self->rank + 1)
                let_lose[j] = 1;
            else
                let_lose[j] = 0;
        }

        if (in_game) {
            for (j = 0; j < s->teams_per_game; j++) {
                opponent = s->teams[s->games[i][j]];

                if (opponent == self)
                    /* self gewinnen lassen
                       -> self bekommt genug Tore um bei
                          Punktegleichheit "besser" zu sein */
                    s->goals[i][j] = MAX(s->ranking[0]->goals - self->goals,
                                         0) + 1;
                else
                    s->goals[i][j] = 0;
            }

            game_evaluation(s, i);
            update_ranking(s);
            continue;
        }

        /* "bessere" Mannschaften verlieren lassen */
        for (j = 0; j < s->teams_per_game; j++) {
            if (let_lose[j])
                s->goals[i][j] = 0;
            else
                s->goals[i][j] = 1;
        }

        game_evaluation(s, i);
        update_ranking(s);
    }

    free(let_lose);
    return self->rank;
}

/* "berechne" schlechtesten noch erreichbaren Platz */
int calculate_worst_rank(struct saison *s, char *teamname)
{
    int i, j;
    struct team *self, *opponent;

    /* self = zu untersuchende Mannschaft */
    if (!(self = get_team(s, teamname))) {
        fprintf(stderr, "Mannschaft '%s' unbekannt\n", teamname);
        return -1;
    }

    for (i = 0; i < s->game_cnt; i++) {
        if (s->goals[i][0] != UNKNOWN)
            continue;

        for (j = 0; j < s->teams_per_game; j++) {
            opponent = s->teams[s->games[i][j]];

            /* Mannschaft verlieren lassen */
            s->goals[i][j] = 0;

            if (opponent == self)
                continue;

            /* "schlechtere" Mannschaften gewinnen lassen */
            if (opponent->rank >= self->rank)
                s->goals[i][j] = 1;
        }

        game_evaluation(s, i);
        update_ranking(s);
    }

    return self->rank;
}

struct saison *saison_copy(struct saison *s)
{
    struct saison *copy;
    int i;

    copy = saison_new();

    /* Teams kopieren */
    for (i = 0; i < s->team_cnt; i++)
        add_team(copy, s->teams[i]->name);

    copy->teams_per_game = s->teams_per_game;
    copy->query          = strdup(s->query);
    init_saison(copy);

    for (i = 0; i < s->game_cnt; i++)
        memcpy(copy->goals[i], s->goals[i], s->teams_per_game * sizeof(int));
    saison_evaluation(copy);

    return copy;
}
