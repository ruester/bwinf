/* 30. Bundeswettbewerb Informatik - 2. Runde
 * Aufgabe 2 - Kool
 *
 * Autor: Matthias Rüster
 *
 * E-Mail: ruester@molgen.mpg.de
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <getopt.h>
#include <limits.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <pthread.h>

#include "labyrinth.h"

#define LABFILE "zufall.lab"
#define PNGFILE "zufall.png"

#define TRY_COUNT 3000000

/* globale Variable, ob ein Thread
   ein gueltiges Labyrinth gefunden hat */
int solved;

static void usage(void)
{
    puts("\nkool [OPTIONEN] <Dateiname>\n\n"

         "moegliche Optionen:\n"
         "  -r                     Rotation erlauben\n\n"

         "  -g                     erlaube auch Sackgassen, die\n"
         "                         nicht aus Sackgassenwuerfeln bestehen\n\n"

         "  -a                     durchlaufe alle optimalen Rechtecke\n\n"

         "  -e <Breite> <Hoehe>    erzeuge zufaelliges Labyrinth\n\n"

         "  -s <Wert>              Anzahl Schritte festlegen\n"
         "                         (Standardwert: 3000000)\n\n"

         "  -c <Anzahl>            Anzahl der Threads festlegen\n\n"

         "Bestimmtes Labyrinth suchen:\n"
         "  -b <Breite>            Breite festlegen\n"
         "  -h <Hoehe>             Hoehe festlegen\n");
}

int get_cpus(void)
{
    int n;

    n = get_nprocs();

    return (n > 0) ? n : 1;
}

void *thread_solve(void *l)
{
    struct labyrinth *lab;
    char filename[LINE_MAX];

    lab = l;

    labyrinth_solve(lab);

    /* wurde ein gueltiges Labyrinth gefunden */
    if (lab->solution) {
        solved = 1;

        sprintf(filename, "%dx%d.png",
                lab->solution->width, lab->solution->height);

        labyrinth2png(lab->solution, filename);

        printf("gefundenes Labyrinth wurde in %s gespeichert.\n", filename);

        /* Speicher freigeben und Loesungsvariable
           wieder zuruecksetzen */
        rectangle_remove(lab->solution);
        lab->solution = NULL;
    }

    pthread_exit(NULL);
}

void *noop(void *a) { pthread_exit(NULL); }

int main(int argc, char *argv[])
{
    struct labyrinth **lab;
    int opt, rotation, generator, verbose, i, j, v, help, alley,
        cpu_count, s, try_count, started, width, height, all;
    pthread_t *thread;
    pthread_attr_t attr;
    void *status;
    int *index;
    struct sizes *size;

    struct option options[] = {
        {"rotation",   no_argument,       &rotation,  1},
        {"gassen",     no_argument,       &alley,     1},
        {"erzeuge",    no_argument,       &generator, 1},
        {"schritte",   required_argument, NULL,       's'},
        {"count",      required_argument, NULL,       'c'},
        {"breite",     required_argument, NULL,       'b'},
        {"hoehe",      required_argument, NULL,       'h'},
        {"verbose",    no_argument,       &verbose,   1},
        {"alle",       no_argument,       &all,       1},
        {"help",       no_argument,       &help,      1},
        {0, 0, 0, 0} };

    /* Standardwerte */
    rotation  = generator = verbose = help = alley = solved = all = 0;
    height    = width     = -1;
    try_count = TRY_COUNT;
    cpu_count = get_cpus();

    srand(time(NULL) + getpid());

    /* Argumente einlesen */
    while ((opt = getopt_long_only(argc, argv, "h:", options, NULL)) != -1) {
        switch (opt) {
            case 's':
                if ((try_count = atof(optarg)) <= 0) {
                    fprintf(stderr,
                            "Anzahl der Schritte muss groesser als 0 sein\n");
                    return 1;
                }
            break;

            case 'c':
                if ((cpu_count = atoi(optarg)) < 1) {
                    fprintf(stderr,
                            "Anzahl der Threads muss groesser als 0 sein\n");
                    return 1;
                }
            break;

            case 'b':
                if ((width = atoi(optarg)) < 1) {
                    fprintf(stderr, "Breite muss groesser als 1 sein\n");
                    return 1;
                }
            break;

            case 'h':
                if ((height = atoi(optarg)) < 1) {
                    fprintf(stderr, "Hoehe muss groesser als 1 sein\n");
                    return 1;
                }
            break;

            case '?':
                exit(EXIT_FAILURE);
        }
    }

    if (help) {
        usage();
        return 0;
    }

    if (generator) {
        /* Anzahl der Argumente pruefen */
        if (optind != argc - 2) {
            if (optind < argc - 2)
                fprintf(stderr, "Zu viele Argumente\n");
            else
                fprintf(stderr, "Breite und Hoehe benoetigt\n");

            usage();
            return 1;
        }

        if ((lab = calloc(1, sizeof(*lab))) == NULL) {
            perror("calloc");
            exit(EXIT_FAILURE);
        }

        if ((width = atoi(argv[optind])) <= 1
            || (height = atoi(argv[optind + 1])) <= 1) {
            fprintf(stderr, "Breite und Hoehe muss groesser als 1 sein\n");
            exit(EXIT_FAILURE);
        }

        lab[0] = labyrinth_generate(width, height);

        /* Labyrinth in Textdatei speichern */
        labyrinth2file(lab[0], LABFILE);

        /* Labyrinth als Bild abspeichern */
        labyrinth2png(lab[0]->solution, PNGFILE);

        labyrinth_remove(lab[0]);
        free(lab);

        printf("Erzeugtes Labyrinth wurde in %s und %s gespeichert.\n",
               LABFILE, PNGFILE);

        return 0;
    }

    /* Anzahl der Argumente pruefen */
    if (optind != argc - 1) {
        if (optind < argc)
            fprintf(stderr, "Zu viele Argumente\n");
        else
            fprintf(stderr, "Dateiname benoetigt\n");

        usage();
        return 1;
    }

    if ((lab = calloc(cpu_count, sizeof(*lab))) == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    /* Labyrinthe initialisieren */
    for (i = 0; i < cpu_count; i++) {
        /* Labyrinth aus Datei einlesen */
        lab[i] = labyrinth_load(argv[optind]);

        /* Parameter eintragen */
        lab[i]->rotation  = rotation;
        lab[i]->try_count = try_count;
        lab[i]->verbose   = verbose;
        lab[i]->alley     = alley;
    }

    if (!alley && !is_solvable(lab[0])) {
        fprintf(stderr, "Wuerfel koennen nicht zu einem gueltigen "
                        "Labyrinth zusammengefuegt werden\n");
        exit(EXIT_FAILURE);
    }

    if ((size = calloc(1, sizeof(*size))) == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    if (height != -1 || width != -1) {
        if (height == -1)
            height = width;

        if (width == -1)
            width = height;

        if (height * width < lab[0]->cube_cnt) {
            fprintf(stderr, "Labyrinth %dx%d ist zu klein fuer %d Wuerfel\n",
                    width, height, lab[0]->cube_cnt);
            return 1;
        }

        add_size(size, width, height);
    } else {
        init_sizes(size, lab[0]->cube_cnt);
    }

    if ((thread = calloc(size->count, sizeof(*thread))) == NULL
        || (index = calloc(size->count, sizeof(*index))) == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    for (v = 0; v < 4; v++) {
        started = s = 0;

        for (i = 0; i < size->count; i++) {
            for (j = started - i;
                 j < cpu_count && started < size->count;
                 j++) {
                lab[s]->width  = size->width[started];
                lab[s]->height = size->height[started];
                index[started] = s;

                /* Threads starten */
                if (pthread_create(&thread[started], &attr,
                                   (!all && solved) ? noop : thread_solve,
                                   (void *) lab[s])) {
                    perror("pthread_create");
                    exit(EXIT_FAILURE);
                }

                started++;
                s++;

                if (s == cpu_count)
                    s = 0;
            }

            /* auf Beendigung des Threads warten */
            if (pthread_join(thread[i], &status)) {
                perror("pthread_join");
                exit(EXIT_FAILURE);
            }
        }

        if (rotation || solved || v == 3)
            break;

        for (i = 0; i < cpu_count; i++)
            for (j = 0; j < lab[i]->cube_cnt; j++)
                cube_rotate(lab[i]->cubes[j], 1);

        printf("\nalle Wuerfel wurden um 90 Grad gedreht\n\n");
    }

    if (!solved)
        printf("\nEs konnte kein gueltiges Labyrinth gefunden werden.\n");

    for (i = 0; i < cpu_count; i++)
        labyrinth_remove(lab[i]);
    free(lab);

    free(thread);
    free(index);

    free(size->width);
    free(size->height);
    free(size);

    pthread_attr_destroy(&attr);

    pthread_exit(NULL);
}
