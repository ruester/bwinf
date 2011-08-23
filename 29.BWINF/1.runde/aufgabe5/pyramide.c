/* Bundeswettbewerb Informatik
 * Aufgabe 5 - Pyramide
 *
 * Autor: Matthias Rüster
 *
 * E-Mail: ruester@molgen.mpg.de
 */

#include <stdlib.h>
#include <stdio.h>

#include "block.h"
#include "koordinaten.h"
#include "pyramide.h"

/* Beschreibung:
 *     setzt bei allen Bloecken der Pyramide
 *     das existiert-Bit und den Blocktyp
 * Eingabewerte:
 *     Zeiger auf die Pyramidenstruktur
 * Rueckgabewerte:
 *     keine */
void baue_pyramide(struct pyramide *p)
{
    int i, j, k;

    for (k = 0; k < p->schichten; k++)
        for (i = 0; i < k + 1; i++)
            for (j = 0; j < k + 1; j++) {
                setze_existiert(&(p->block[k][i][j]));
                setze_blocktyp(&(p->block[k][i][j]),
                               bestimme_blocktyp(k, i, j));
            }
}

/* Beschreibung:
 *     prueft, ob ein Block einer Pyramide blockiert ist
 * Eingabewerte:
 *     1. Argument: Zeiegr auf die Pyramidenstruktur
 *     2. - 4. Argument: Koordinaten des Blocks
 * Rueckgabewerte:
 *     0, wenn der Block nicht blockiert ist
 *     1, wenn der Block blockiert ist */
char ist_blockiert(struct pyramide *p, int k, int x, int y)
{
    int i;

    if (k == 0)
        return (0);             /* Spitze */

    k--;

    for (i = 0; i < 4; i++) {
        switch (i) {
        case 1:
            x--;
            break;

        case 2:
            y--;
            break;

        case 3:
            x++;
            break;
        }

        if (y < 0)
            break;

        if (x < 0 || x > k || y > k)
            continue;

        if (existiert(p->block[k][x][y]))
            return (1);
    }

    return (0);
}

/* Beschreibung:
 *     - findet die abholbaren Bloecke einer Pyramide
 *     - die gefundenen Bloecke werden in der Koordinatenliste gespeichert
 *     - es wird maximal nur ein Stein jeden Typs gefunden
 * Eingabewerte:
 *     Zeiger auf die Pyramidenstruktur
 * Rueckgabewerte:
 *     keine */
void finde_abholbare_bloecke(struct pyramide *p)
{
    struct koordinate *ko;
    int k, x, y;
    char eckstein, kantenstein, mittelstein, typ;

    eckstein = kantenstein = mittelstein = 0;

    for (k = 0; k < p->schichten; k++) {
        for (x = 0; x < k + 1; x++) {
            for (y = 0; y < k + 1; y++) {
                if (eckstein && kantenstein && mittelstein)
                    return;
                
                if (!existiert(p->block[k][x][y]))
                    continue;

                typ = blocktyp(p->block[k][x][y]);

                if ((typ == ECKSTEIN && eckstein)
                    || (typ == KANTENSTEIN && kantenstein)
                    || (typ == MITTELSTEIN && mittelstein))
                    continue;

                if (!ist_blockiert(p, k, x, y)) {
                    ko = neue_koordinate(k, x, y);
                    koordinate_hinzufuegen(p->kl, ko);
                    
                    if (typ == ECKSTEIN)
                        eckstein = 1;
                    
                    if (typ == KANTENSTEIN)
                        kantenstein = 1;
                    
                    if (typ == MITTELSTEIN)
                        mittelstein = 1;
                }
            }
        }
    }
}

/* Beschreibung:
 *     prueft, ob ein Block von (vier) Bloecken gestuetzt wird
 * Eingabewerte:
 *     1. Argument: Zeiger auf die Pyramidenstruktur
 *     2. - 4. Argument: Koordinaten des Blocks
 * Rueckgabewerte:
 *     0, wenn der Block nicht gestuetzt ist
 *     1, wenn der Block gestuetzt ist */
char ist_gestuetzt(struct pyramide *p, int k, int x, int y)
{
    int i;

    if (k + 1 == p->schichten)
        return (1);             /* Boden */

    /* alle 4 Bloecke darunter auf Existenz pruefen */
    k++;

    for (i = 0; i < 4; i++) {
        switch (i) {
        case 1:
            x++;
            break;

        case 2:
            y++;
            break;

        case 3:
            x--;
            break;
        }

        if (!existiert(p->block[k][x][y]))
            return (0);         /* keine 4 stuetzenden Bloecke */
    }

    /* 4 stuetzende Bloecke sind vorhanden */
    return (1);
}

/* Beschreibung:
 *     holt Speicher fuer eine Pyramidenstruktur
 * Eingabewerte:
 *     Anzahl der Schichten der Pyramide
 * Rueckgabewerte:
 *     Zeiger auf die erstellte Pyramidenstruktur */
struct pyramide *neue_pyramide(int n)
{
    struct pyramide *p;
    int i, j;

    if ((p = calloc(1, sizeof(struct pyramide))) == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    p->schichten = n;

    if ((p->block = calloc(n, sizeof(int **))) == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < n; i++) {
        if ((p->block[i] = calloc(i + 1, sizeof(int *))) == NULL) {
            perror("calloc");
            exit(EXIT_FAILURE);
        }

        for (j = 0; j < i + 1; j++) {
            if ((p->block[i][j] = calloc(i + 1, sizeof(int))) == NULL) {
                perror("calloc");
                exit(EXIT_FAILURE);
            }
        }
    }

    p->kl = neue_koordinatenliste();

    return (p);
}

/* Beschreibung:
 *     gibt den Speicher fuer eine Pyramidenstruktur frei
 * Eingabewerte:
 *     Zeiger auf die Pyramidenstruktur
 * Rueckgabewerte:
 *     keine */
void pyramide_freigeben(struct pyramide *p)
{
    int i, j;

    loesche_koordinatenliste(p->kl);

    for (i = 0; i < p->schichten; i++) {
        for (j = 0; j < i + 1; j++)
            free(p->block[i][j]);
        free(p->block[i]);
    }

    free(p->block);
    free(p);
}

/* Beschreibung:
 *     aktualisiert die Liste abholbarer Steine
 * Eingabewerte:
 *     Zeiger auf die Pyramidenstruktur
 * Rueckgabewerte:
 *     keine */
void update_liste(struct pyramide *p)
{
    loesche_koordinatenliste(p->kl);
    p->kl = neue_koordinatenliste();
    finde_abholbare_bloecke(p);
}

/* Beschreibung:
 *     prueft ob die Pyramide fertig gebaut wurde
 * Eingabewerte:
 *     Zeiger auf die Pyramidenstruktur
 * Rueckgabewerte:
 *     1, wenn Pyramide fertig gebaut ist
 *     0, wenn die Pyramide nicht fertig gebaut ist */
char ist_fertig(struct pyramide *p)
{
    int k, x, y;

    for (k = 0; k < p->schichten; k++)
        for (x = 0; x < k + 1; x++)
            for (y = 0; y < k + 1; y++)
                if (!existiert(p->block[k][x][y]))
                    return (0);

    return (1);
}

/* Beschreibung:
 *     prueft ob ein Block eines bestimmten Typs
 *     von der Pyramide abgeholt werden kann
 * Eingabewerte:
 *    1. Argument: Zeiger auf die Pyramidenstruktur
 *    2. Argument: der Blocktyp
 * Rueckgabewerte:
 *    Zeiger auf die Koordinate des Blocks, wenn der Block geholt werden kann
 *    NULL, wenn der Block nicht geholt werden kann */
struct koordinate *hole_block(struct pyramide *p, char typ)
{
    struct koordinate *ko, *temp, *ret;

    ko = NULL;

    /* Liste der abholbaren Bloecke aktualisieren */
    update_liste(p);

    temp = p->kl->first;

    /* Koordinatenliste durchlaufen */
    while (temp != NULL) {
        if (blocktyp(p->block[temp->k][temp->x][temp->y]) == typ) {
            ko = temp; /* Block gefunden */
            break;
        }

        temp = temp->next;
    }

    if (ko == NULL)
        return (NULL); /* es wurde kein Block gefunden */

    /* Kopie der Koordinate anlegen */
    ret = neue_koordinate(ko->k, ko->x, ko->y);

    /* Zeiger auf die Koordinate zurueckgeben */
    return (ret);
}

/* Beschreibung:
 *     prueft, ob ein Block eines bestimmten Typs abholbar ist
 * Eingabewerte:
 *     1. Argument: Zeiger auf die Pyramidenstruktur
 *     2. Argument: der Blocktyp
 * Rueckgabewerte:
 *  */
int ist_abholbar(struct pyramide *p, int typ)
{
    struct koordinate *temp;

    temp = p->kl->first;

    /* Koordinatenliste durchlaufen */
    while (temp != NULL) {
        if (blocktyp(p->block[temp->k][temp->x][temp->y]) == typ)
            return (1); /* Blocktyp ist in der Liste enthalten */
        temp = temp->next;
    }

    /* Blocktyp ist nicht in der Liste enthalten */
    return (0);
}
