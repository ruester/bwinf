/* Bundeswettbewerb Informatik
 * Aufgabe 5 - Pyramide
 *
 * Autor: Matthias Rüster
 *
 * E-Mail: ruester@molgen.mpg.de
 */

#ifndef AUFGABE5_BLOCK_H
#define AUFGABE5_BLOCK_H

#define EXISTIERT   (1 << 0)
#define MITTELSTEIN (1 << 1)
#define KANTENSTEIN (1 << 2)
#define ECKSTEIN    (1 << 3)
#define SPITZE      (1 << 4)
#define BLOCKTYP    (MITTELSTEIN | KANTENSTEIN | ECKSTEIN | SPITZE)

extern void setze_existiert_nicht(char *block);
extern void setze_existiert(char *block);
extern void setze_blocktyp(char *block, char typ);
extern char existiert(char block);
extern char blocktyp(char block);
extern char bestimme_blocktyp(int k, int x, int y);

#endif
