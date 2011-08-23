/* Bundeswettbewerb Informatik
 * Aufgabe 2 - Robuttons
 *
 * Autor: Matthias Rüster
 *
 * E-Mail: ruester@molgen.mpg.de
 */

#ifndef AUFGABE2_KOLLISION_H
#define AUFGABE2_KOLLISION_H

#include "simulation.h"

extern char pruefe_kollision_tischkante_eckig(struct simulation *s, int i);

extern char pruefe_kollision_tischkante_rund(struct simulation *s, int i);

extern char pruefe_kollision_robuttons(struct simulation *s, int i);

extern char pruefe_kollision_muenzen(struct simulation *s, int i);

#endif
