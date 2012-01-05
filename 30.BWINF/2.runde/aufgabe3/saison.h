/* 30. Bundeswettbewerb Informatik - 2. Runde
 * Aufgabe 3 - Trickey
 *
 * Autor: Matthias Rüster
 *
 * E-Mail: ruester@molgen.mpg.de
 */

#ifndef AUFGABE3_SAISON_H
#define AUFGABE3_SAISON_H

struct saison {
    struct team **teams;   /* alle Mannschaften */
    struct team **ranking; /* Manschaften sortiert nach Platzierung */
    int team_cnt;          /* Anzahl der Mannschaften */

    /* die Informationen ueber die Spiele/Spielergebnisse */
    int **games,        /* Mannschaftskombinationen (jeder gegen jeden) */
        **goals;        /* geschossene Tore */
    int game_cnt,       /* Anzahl der Spiele, die gespielt werden muessen */
        teams_per_game; /* Anzahl der Mannschaften auf dem Feld */

    char *query; /* Name der Mannschaft, die "untersucht" werden soll */
};

extern struct saison *saison_load(char *fn);
extern void saison_print(struct saison *s);
extern void saison_free(struct saison *s);
extern int calculate_best_rank(struct saison *s, char *teamname);
extern int calculate_worst_rank(struct saison *s, char *teamname);
extern struct saison *saison_copy(struct saison *s);

#endif
