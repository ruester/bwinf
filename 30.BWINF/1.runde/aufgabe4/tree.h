/* 30. Bundeswettbewerb Informatik
 * Aufgabe 4 - Zaras dritter Fehler
 *
 * Autor: Matthias Rüster
 *        Lena Kristin Zander
 *
 * E-Mail: ruester@molgen.mpg.de
 *         zander@molgen.mpg.de
 */

#ifndef AUFGABE4_TREE_H
#define AUFGABE4_TREE_H

#include <wchar.h>

#include "entry.h"

struct tree {
    struct tree_node *root;
};

struct tree_node {
    struct entry *n;
    struct tree_node *left, *right, *parent;
    unsigned char max_depth;
    char balance_value;
};

extern struct tree *tree_new();
extern void tree_insert(struct tree *root, struct entry *n);
extern struct tree_node *tree_search_tree_node(struct tree *t,
                                               struct entry *n);
extern struct entry *tree_search_node(struct tree *t, wchar_t *name);
extern void tree_node_free(struct tree *t, struct tree_node *n);
extern unsigned long tree_count(struct tree *t);
extern void tree_free_all_nodes(struct tree *t);
extern void tree_free(struct tree *t);
extern struct tree_node *tree_next(struct tree_node *t);
extern struct tree_node *tree_first(struct tree_node *t);

#endif
