/* 30. Bundeswettbewerb Informatik
 * Aufgabe 4 - Zaras dritter Fehler
 *
 * Autor: Matthias Rüster
 *        Lena Kristin Zander
 *
 * E-Mail: ruester@molgen.mpg.de
 *         zander@molgen.mpg.de
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

#include "entry.h"

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

struct entry *entry_new(wchar_t *name)
{
    struct entry *n;
    int l;

    if ((n = calloc(1, sizeof(*n))) == NULL) {
        perror("entry_new: calloc");
        exit(EXIT_FAILURE);
    }

    l = wcslen(name);

    if ((n->name = calloc(l + 1, sizeof(*(n->name)))) == NULL) {
        perror("entry_new: calloc");
        exit(EXIT_FAILURE);
    }

    memcpy(n->name, name, sizeof(*(n->name)) * (l + 1));

    return n;
}

void entry_free(struct entry *n)
{
    free(n->name);
    free(n);
}

int entrycmp(struct entry *a, struct entry *b)
{
    if (a == b)
        return 0;

    return wcscmp(a->name, b->name);
}
