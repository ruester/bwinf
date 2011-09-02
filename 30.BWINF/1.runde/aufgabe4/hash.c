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
#include <wchar.h>

#include "hash.h"

struct hash *hash_new(void)
{
    struct hash *h;
    unsigned long i;

    if ((h = calloc(1, sizeof(*h))) == NULL) {
        perror("hash_new: calloc");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < TBLSIZE; i++)
        h->tbl[i] = tree_new();

    return h;
}

unsigned long hash_index(wchar_t *key)
{
    unsigned long index = 0;
    wchar_t c;

    while ((c = *key++))
        index = c + (index << 6) + (index << 16) - index;

    return index % TBLSIZE;
}

void hash_insert(struct hash *hash, struct entry *n)
{
    unsigned long index = hash_index(n->name);

    tree_insert(hash->tbl[index], n);

    hash->cnt++;
}

struct entry *hash_search(struct hash *hash, wchar_t *key)
{
    return tree_search_node(hash->tbl[hash_index(key)], key);
}

void hash_free(struct hash *hash)
{
    int i;

    for (i = 0; i < TBLSIZE; i++) {
        tree_free_all_nodes(hash->tbl[i]);
        tree_free(hash->tbl[i]);
    }

    free(hash);
}
