/* Bundeswettbewerb Informatik
 * Aufgabe 5 - Pyramide
 *
 * Autor: Matthias Rüster
 *
 * E-Mail: ruester@molgen.mpg.de
 */

#include <stdlib.h>
#include <stdio.h>

#include "umzug.h"
#include "transport.h"
#include "pyramide.h"
#include "block.h"

/* Beschreibung:
 *     - der Koordinatenliste werden Bloecke hinzugefuegt,
 *       die zum Stuetzen des Blocks notwendig sind
 *     - fuer jeden der hinzugefuegten Bloecke wird der Blocktyp gesetzt
 * Eingabewerte:
 *     1. Argument: Zeiger auf die Koordinatenliste
 *     2. Argument: Zeiger auf die Pyramidenstruktur
 *     3. - 5. Argument: Koordinaten des Blocks
 * Rueckgabewerte:
 *     keine */
static void steinblock_planen(struct pyramide *p, int k, int x, int y)
{
    if (k == p->schichten || p->block[k][x][y] != 0)
        return;

    /* eine Schicht tiefer gehen */
    steinblock_planen(p, k + 1, x,     y);
    steinblock_planen(p, k + 1, x + 1, y);
    steinblock_planen(p, k + 1, x,     y + 1);
    steinblock_planen(p, k + 1, x + 1, y + 1);

    koordinate_hinzufuegen(p->kl, neue_koordinate(k, x, y));

    /* Blocktyp setzen */
    setze_blocktyp(&(p->block[k][x][y]), bestimme_blocktyp(k, x, y));
}

/* Beschreibung:
 *     erstellt einen Bauplan (Koordinatenliste) fuer die Pyramide
 * Eingabewerte:
 *     Zeiger auf die Pyramidenstruktur
 * Rueckgabewerte:
 *     Zeiger auf die Koordinatenliste */
static void init_bauplan(struct pyramide *p)
{
    int k;

    /* Bauplan erstellen */
    for (k = p->schichten - 1; k >= 0; k--) {
        steinblock_planen(p, k, 0, 0);
        steinblock_planen(p, k, 0, k);
        steinblock_planen(p, k, k, 0);
        steinblock_planen(p, k, k, k);
    }
}

/* Beschreibung:
 *     holt Speicher fuer einen neuen Pyramidenumzug
 * Eingabewerte:
 *     Anzahl der Schichten der Pyramiden
 * Rueckgabewerte:
 *     Zeiger auf den erstellten Pyramidenumzug */
struct pyramidenumzug *neuer_pyramidenumzug(int n)
{
    struct pyramidenumzug *u;

    /* Speicher holen */
    if ((u = calloc(1, sizeof(struct pyramidenumzug))) == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    /* Pyramiden erstellen */
    u->theben = neue_pyramide(n);
    u->luxor = neue_pyramide(n);
    
    /* eine Pyramide bauen */
    baue_pyramide(u->theben);
    
    /* Bauplan erstellen */
    init_bauplan(u->luxor);

    /* Transportliste erstellen */
    u->umzug = neue_transportliste();

    /* Zeiger zurueckgeben */
    return (u);
}

/* Beschreibung:
 *     erstellt die Anleitung zum Umzug der Pyramide
 * Eingabewerte:
 *     Zeiger auf den Pyramidenumzug
 * Rueckgabewerte:
 *     keine */
void umzug(struct pyramidenumzug *u)
{
    struct transport *t;
    struct koordinate *holen, *b;
    char typ;

    /* Spezialfall: die Pyramide besteht nur aus der Spitze */
    if (u->theben->schichten == 1) {
        /* neuen Transport erstellen */
        t = neuer_transport(0, 0, 0, 0, 0, 0, SPITZE);
        
        /* Block bewegen */
        setze_existiert_nicht(&(u->theben->block[0][0][0]));
        setze_existiert(&(u->luxor->block[0][0][0]));
        
        /* Transport speichern */
        transport_hinzufuegen(u->umzug, t);
        
        return;
    }

    /* Spitze ins Lager bringen */
    t = neuer_transport(0, 0, 0, LAGER, LAGER, LAGER, SPITZE);
    
    /* Transport speichern */
    transport_hinzufuegen(u->umzug, t);
    
    /* Spitze wegnehmen */
    setze_existiert_nicht(&(u->theben->block[0][0][0]));

    /* den Bauplan durchlaufen */
    b = u->luxor->kl->first;

    while (b != NULL) {
        /* Blocktyp vom zu platzierenden Block bestimmen */
        typ = blocktyp(u->luxor->block[b->k][b->x][b->y]);

        /* pruefen, ob Blocktyp geholt werden kann */
        if ((holen = hole_block(u->theben, typ)) != NULL) {
            /* pruefen, ob Block platziert werden darf */
            if (!ist_gestuetzt(u->luxor, b->k, b->x, b->y)) {
                /* naechster Block */
                b = b->next;
                continue;
            }

            /* Block bewegen */
            setze_existiert_nicht(&(u->theben->block[holen->k]
                                    [holen->x]
                                    [holen->y]));
            setze_existiert(&(u->luxor->block[b->k][b->x][b->y]));
            
            /* neuen Transport erstellen */
            t = neuer_transport(holen->k, holen->x, holen->y,
                                b->k, b->x, b->y, typ);
            
            /* Transport speichern */
            transport_hinzufuegen(u->umzug, t);
            
            /* Koordinate aus dem Bauplan loeschen */
            loesche_koordinate(u->luxor->kl, b);
            
            /* Speicher der Koordinate freigeben */
            free(holen);
            
            /* wieder von vorne anfangen */
            b = u->luxor->kl->first;
        } else
            b = b->next; /* naechster Block */
    }

    /* Spitze vom Lager holen */
    t = neuer_transport(LAGER, LAGER, LAGER, 0, 0, 0, SPITZE);
    
    /* Transport speichern */
    transport_hinzufuegen(u->umzug, t);
    
    /* Spitze platzieren */
    setze_existiert(&(u->luxor->block[0][0][0]));
}

/* Beschreibung:
 *     gibt den Speicher eines Pyramidenumzugs frei
 * Eingabewerte:
 *     Zeiger auf den Pyramidenumzug
 * Rueckgabewerte:
 *     keine */
void pyramidenumzug_freigeben(struct pyramidenumzug *u)
{
    transportliste_freigeben(u->umzug);

    /* Speicher der Pyramidenstrukturen freigeben */
    pyramide_freigeben(u->theben);
    pyramide_freigeben(u->luxor);

    /* Speicher der Struktur freigeben */
    free(u);
}
