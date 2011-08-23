/* Bundeswettbewerb Informatik
 * Aufgabe 3 - Logistik
 *
 * Autor: Matthias Rüster
 *
 * E-Mail: ruester@molgen.mpg.de
 */

#ifndef AUFGABE3_LOGISTIK_H
#define AUFGABE3_LOGISTIK_H

extern int **auftragsbuch_erstellen(char *dateiname);

extern void ohne_leerfahrten(int **auftragsbuch);

extern void mit_leerfahrten(int **auftragsbuch);

extern void auftragsbuch_freigeben(int **auftragsbuch);

extern void print_auftragsbuch(int **auftragsbuch);

#endif
