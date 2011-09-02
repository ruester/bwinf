/* 30. Bundeswettbewerb Informatik
 * Aufgabe 4 - Zaras dritter Fehler
 *
 * Autor: Matthias Rüster
 *        Lena Kristin Zander
 *
 * E-Mail: ruester@molgen.mpg.de
 *         zander@molgen.mpg.de
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "tree.h"

#define MIN(a, b) (((a) < (b)) ? (a)  : (b))
#define MAX(a, b) (((a) > (b)) ? (a)  : (b))
#define ABS(a)    (((a) < 0)   ? (-a) : (a))

struct tree_node *tree_node_new(struct entry *n)
{
    struct tree_node *t;

    if ((t = calloc(1, sizeof(*t))) == NULL) {
        perror("tree_node_new: calloc");
        exit(EXIT_FAILURE);
    }

    t->n = n;

    return t;
}

struct tree *tree_new()
{
    struct tree *t;

    if ((t = calloc(1, sizeof(*t))) == NULL) {
        perror("tree_new: calloc");
        exit(EXIT_FAILURE);
    }

    return t;
}

void tree_node_update_values(struct tree_node *root)
{
    unsigned char depth_left, depth_right;

    do {
        depth_left  = root->left  ? root->left->max_depth  + 1 : 0;
        depth_right = root->right ? root->right->max_depth + 1 : 0;
        root->max_depth     = MAX(depth_left, depth_right);
        root->balance_value = depth_left - depth_right;
        root = root->parent;
    } while (root);
}

struct tree_node *tree_node_rotate_left(struct tree_node *root)
{
    struct tree_node *pivot;

    if (!root->right)
        return root;

    pivot         = root->right;
    pivot->parent = root->parent;

    root->right = pivot->left;
    if (root->right)
        root->right->parent = root;

    pivot->left  = root;
    root->parent = pivot;

    if (pivot->parent) {
        if (pivot->parent->left == root)
            pivot->parent->left  = pivot;
        else
            pivot->parent->right = pivot;
    }

    tree_node_update_values(root);

    return pivot;
}

struct tree_node *tree_node_rotate_right(struct tree_node *root)
{
    struct tree_node *pivot;

    if (!root->left)
        return root;

    pivot         = root->left;
    pivot->parent = root->parent;

    root->left = pivot->right;
    if (root->left)
        root->left->parent = root;

    pivot->right = root;
    root->parent = pivot;

    if (pivot->parent) {
        if (pivot->parent->left == root)
            pivot->parent->left  = pivot;
        else
            pivot->parent->right = pivot;
    }

    tree_node_update_values(root);

    return pivot;
}

struct tree_node *tree_node_balance(struct tree_node *n)
{
    struct tree_node *tn;

    if (ABS(n->balance_value) < 2) {
        if (n->parent)
            return tree_node_balance(n->parent);
        return n;
    }

    if (n->balance_value > 1) {
        /* Doppelroatation links-rechts */
        if (n->left->balance_value < 0)
            n->left = tree_node_rotate_left(n->left);

        /* Einfachrotation links */
        tn = tree_node_rotate_right(n);
    } else {
        /* Doppelrotation rechts-links */
        if (n->right->balance_value > 0)
            n->right = tree_node_rotate_right(n->right);

        /* Einfachrotation links */
        tn = tree_node_rotate_left(n);
    }

    if (tn->parent)
        return tree_node_balance(tn->parent);
    return tn;
}

void tree_insert(struct tree *t, struct entry *n)
{
    int comp;
    struct tree_node *root;

    if (!t->root) {
        t->root = tree_node_new(n);
        return;
    }

    root = t->root;

    while (root) {
        comp = entrycmp(root->n, n);

        if (comp < 0) {
            if (!root->left) {
                root->left         = tree_node_new(n);
                root->left->parent = root;
                break;
            }

            root = root->left;
            continue;
        }

        if (comp > 0) {
            if (!root->right) {
                root->right         = tree_node_new(n);
                root->right->parent = root;
                break;
            }

            root = root->right;
            continue;
        }

        return;
    }

    tree_node_update_values(root);
    root = tree_node_balance(root);

    if (!root->parent)
        t->root = root;
}

struct tree_node *tree_search_tree_node(struct tree *t, struct entry *n)
{
    int comp;
    struct tree_node *root;

    root = t->root;

    while (root) {
        comp = entrycmp(root->n, n);

        if (comp < 0) {
            root = root->left;
            continue;
        }

        if (comp > 0) {
            root = root->right;
            continue;
        }

        return root;
    }

    return NULL;
}

struct entry *tree_search_node(struct tree *t, wchar_t *name)
{
    int comp;
    struct tree_node *root;
    int g, h;

    root = t->root;

    while (root) {
        g = wcslen(root->n->name);
        h = wcslen(name);

        comp = memcmp(root->n->name, name, (MIN(g, h) + 1) * sizeof(*name));

        if (comp == 0)
            return root->n;

        if (comp < 0) {
            root = root->left;
            continue;
        }

        if (comp > 0) {
            root = root->right;
            continue;
        }
    }

    return NULL;
}

struct tree_node *predecessor(struct tree_node *root)
{
    struct tree_node *current;

    current = root->left;

    if (!current)
        return NULL;

    while (current->right)
        current = current->right;

    return current;
}

void tree_node_free(struct tree *t, struct tree_node *n)
{
    struct tree_node *delete,
                     *replace,
                     *to_update;
    struct entry *s;

    delete = n;

    if (!delete)
        return;

    s = delete->n;

    if (delete->left) {
        if (delete->right) {
            replace = predecessor(delete);

            delete->n = replace->n;
            delete    = replace;

            if (delete->left) {
                delete->n = delete->left->n;
                delete    = delete->left;
            }

        } else {
            delete->n = delete->left->n;
            delete    = delete->left;
        }
    } else {
        if (delete->right) {
            delete->n = delete->right->n;
            delete    = delete->right;
        }
    }

    if ((to_update = delete->parent)) {
        if (to_update->left == delete)
            to_update->left = NULL;
        else
            to_update->right = NULL;
    } else {
        t->root = NULL;
    }

    free(delete);

    if (to_update) {
        tree_node_update_values(to_update);
        replace = tree_node_balance(to_update);
        if (!replace->parent)
            t->root = replace;
    }
}

void tree_count_nodes(struct tree_node *n, unsigned long *cnt)
{
    if (!n)
        return;

    (*cnt)++;

    tree_count_nodes(n->left, cnt);
    tree_count_nodes(n->right, cnt);
}

unsigned long tree_count(struct tree *t)
{
    unsigned long cnt = 0;

    tree_count_nodes(t->root, &cnt);

    return cnt;
}

void free_nodes(struct tree_node *n)
{
    if (!n)
        return;

    free_nodes(n->left);
    free_nodes(n->right);
    entry_free(n->n);
}

void tree_free_all_nodes(struct tree *t)
{
    free_nodes(t->root);
}

void free_tree_nodes(struct tree_node *n)
{
    if (!n)
        return;

    free_tree_nodes(n->left);
    free_tree_nodes(n->right);
    free(n);
}

void tree_free(struct tree *t)
{
    free_tree_nodes(t->root);
    free(t);
}

struct tree_node *tree_next(struct tree_node *t)
{
    struct tree_node *a;

    if (t->right)
        return tree_first(t->right);

    do {
        a = t;
        t = t->parent;
    } while (t && t->right == a);

    return t;
}

struct tree_node *tree_first(struct tree_node *t)
{
    while (t && t->left)
        t = t->left;

    return t;
}
