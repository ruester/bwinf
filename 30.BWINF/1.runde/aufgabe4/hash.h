/* 30. Bundeswettbewerb Informatik
 * Aufgabe 4 - Zaras dritter Fehler
 *
 * Autor: Matthias Rüster
 *        Lena Kristin Zander
 *
 * E-Mail: ruester@molgen.mpg.de
 *         zander@molgen.mpg.de
 */

#ifndef AUFGABE4_HASH_H
#define AUFGABE4_HASH_H

#include <wchar.h>

#include "tree.h"

#define TBLSIZE 2000003 /* Primzahl */

struct hash {
    struct tree *tbl[TBLSIZE];
    unsigned long cnt;
};

extern struct hash *hash_new(void);
extern unsigned long hash_index(wchar_t *key);
extern void hash_insert(struct hash *hash, struct entry *n);
extern struct entry *hash_search(struct hash *hash, wchar_t *key);
extern void hash_free(struct hash *hash);

#endif
