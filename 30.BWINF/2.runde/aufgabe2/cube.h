/* 30. Bundeswettbewerb Informatik - 2. Runde
 * Aufgabe 2 - Kool
 *
 * Autor: Matthias Rüster
 *
 * E-Mail: ruester@molgen.mpg.de
 */

#ifndef AUFGABE2_CUBE_H
#define AUFGABE2_CUBE_H

/* Oeffnungen */
#define NORTH 0
#define EAST  1
#define SOUTH 2
#define WEST  3

/* Anzahl Richtungen */
#define DIRECTIONS 4

/* Typen
Kreuzung => north = 1   east = 1   south = 1 west = 1

             N          E          S         W
      15 =  (1 << 3) & (1 << 2) & (1 << 1) & 1
            => alle Oeffnungen offen -> Kreuzung

      10 =  (1 << 3) & (0 << 2) & (1 << 1) & 0
            => Gerade (mit Oeffnungen im Norden und Sueden)
*/
#define KREUZUNG            15
#define GERADEAUS_OST_WEST  5
#define GERADEAUS_NORD_SUED 10
#define KNICK_NORD_WEST     9
#define KNICK_NORD_OST      12
#define KNICK_OST_SUED      6
#define KNICK_SUED_WEST     3
#define TKREUZUNG_WEST      11
#define TKREUZUNG_NORD      13
#define TKREUZUNG_OST       14
#define TKREUZUNG_SUED      7
#define SACKGASSE_WEST      1
#define SACKGASSE_NORD      8
#define SACKGASSE_OST       4
#define SACKGASSE_SUED      2
#define LEER                0

#define TYPE_CNT 16

struct cube {
    char north, east, south, west;
    char type;
};

extern void cube_init(struct cube *c);
extern struct cube *cube_new(char *type);
extern struct cube *cube_new_by_type(int type);
extern char get_type(struct cube *c);
extern int convert_type(char *type);
extern char *type_to_string(struct cube *c);
extern void cube_rotate(struct cube *c, int count);

#endif
