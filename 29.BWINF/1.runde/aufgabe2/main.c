/* Bundeswettbewerb Informatik
 * Aufgabe 2 - Robuttons
 *
 * Autor: Matthias Rüster
 *
 * E-Mail: ruester@molgen.mpg.de
 */

#include <stdio.h>

#include "simulation.h"

void anleitung()
{
    printf("\nFolgende Tasten koennen waehrend der Simulation\n");
    printf("benutzt werden:\n\n");
    printf("+\t\tHineinzoomen\n\n");
    printf("-\t\tHinauszoomen\n\n");
    printf("Pfeiltasten\tScrollen\n\n");
    printf("Tasten 0 - 9\tFrameskip einstellen:\n");
    printf("\t\t1 entspricht 10%% des max. Frameskip\n");
    printf("\t\t2 entspricht 20%% des max. Frameskip\n");
    printf("\t\t...\n");
    printf("\t\t0 entspricht dem max. Frameskip\n\n");
    printf("Strg (links)\tFrameskip um 1 verringern\n\n");
    printf("Strg (rechts)\tFrameskip um 1 erhoehen\n\n");
    printf("r\t\tFrameskip auf 0 setzen\n\n");
}

int main(int argc, char *argv[])
{
    /* Parameter pruefen */
    if (argc != 2) {
        printf("Konfigurationsdatei als Parameter angeben\n");
        return (1);
    }

    anleitung();

    simulation_starten(argv[1]);

    return (0);
}
