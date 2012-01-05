/* 30. Bundeswettbewerb Informatik - 2. Runde
 * Aufgabe 3 - Trickey
 *
 * Autor: Matthias Rüster
 *
 * E-Mail: ruester@molgen.mpg.de
 */

#include <stdio.h>
#include <getopt.h>

#include "saison.h"

void usage(void)
{
    puts("\ntrickey [OPTIONEN] <Dateiname>\n\n"

         "moegliche Optionen:\n"
         "    -b    Zeige bestmoeglichen Platz\n"
         "    -s    Zeige schlechtesten Platz\n");
}

int main(int argc, char *argv[])
{
    struct saison *s, *copy;
    int rank, best, worst, help, i;
    struct option options[] = {
        {"bester",        no_argument, &best,  1},
        {"schlechtester", no_argument, &worst, 1},
        {"help",          no_argument, &help,  1},
        {0, 0, 0, 0} };

    best = worst = help = 0;

    while (getopt_long_only(argc, argv, "", options, NULL) != -1);

    if (help) {
        usage();
        return 0;
    }

    if (optind >= argc) {
        fprintf(stderr, "Dateiname benoetigt\n");
        usage();
        return 1;
    }

    for (i = optind; i < argc; i++) {
        s = saison_load(argv[i]);

        puts("aktueller Stand der Saison:");
        saison_print(s);

        if (!best && !worst) {
            saison_free(s);
            continue;
        }

        if (best && worst)
            copy = saison_copy(s);
        else
            copy = s;

        if (best) {
            if ((rank = calculate_best_rank(s, s->query)) == -1) {
                saison_free(s);
                return 1;
            }

            printf("Der bestmoegliche Platz fuer "
                   "die Mannschaft %s ist der %d. Platz.\n\n", s->query, rank);

            puts("Dafuer benoetigte Spielergebnisse:");
            saison_print(s);
            saison_free(s);
        }

        if (worst) {
            if ((rank = calculate_worst_rank(copy, copy->query)) == -1) {
                saison_free(copy);
                return 1;
            }

            printf("Der schlechteste Platz fuer "
                   "die Mannschaft %s ist der %d. Platz.\n\n", copy->query, rank);

            puts("Dafuer benoetigte Spielergebnisse:");
            saison_print(copy);
            saison_free(copy);
        }
    }

    return 0;
}
