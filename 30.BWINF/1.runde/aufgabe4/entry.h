/* 30. Bundeswettbewerb Informatik
 * Aufgabe 4 - Zaras dritter Fehler
 *
 * Autor: Matthias Rüster
 *        Lena Kristin Zander
 *
 * E-Mail: ruester@molgen.mpg.de
 *         zander@molgen.mpg.de
 */

#ifndef AUFGABE4_ENTRY_H
#define AUFGABE4_ENTRY_H

#include <wchar.h>

#include "tree.h"

struct entry {
    wchar_t *name;
};

extern struct entry *entry_new(wchar_t *name);
extern void entry_free(struct entry *n);
extern int entrycmp(struct entry *a, struct entry *b);

#endif
