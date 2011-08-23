/* Bundeswettbewerb Informatik
 * Aufgabe 5 - Pyramide
 *
 * Autor: Matthias Rüster
 *
 * E-Mail: ruester@molgen.mpg.de
 */

#include "block.h"

/* Beschreibung:
 *     prueft ob ein Block existiert
 * Eingabewerte:
 *     ein Block
 * Rueckgabewerte:
 *     1, wenn der Block existiert
 *     0, wenn der Block nicht existiert */
char existiert(char block)
{
    return (block & EXISTIERT);
}

/* Beschreibung:
 *     gibt den Blocktyp eines Blocks zurueck
 * Eingabewerte:
 *     ein Block
 * Rueckgabewerte:
 *     den Blocktyp des Blocks */
char blocktyp(char block)
{
    return (block & BLOCKTYP);
}

/* Beschreibung:
 *     loescht das existiert-Bit eines Blocks
 * Eingabewerte:
 *     Zeiger auf den Block
 * Rueckgabewerte;
 *     keine */
void setze_existiert_nicht(char *block)
{
    *block &= ~EXISTIERT;
}

/* Beschreibung:
 *     setzt das existiert-Bit eines Blocks
 * Eingabewerte:
 *     Zeiger auf den Block
 * Rueckgabewerte:
 *     keine */
void setze_existiert(char *block)
{
    *block |= EXISTIERT;
}

/* Beschreibung:
 *     setzt den Blocktyp eines Blocks
 * Eingabewerte:
 *     1. Argument: Zeiger auf den Block
 *     2. Argument: Typ
 * Rueckgabewerte:
 *     keine*/
void setze_blocktyp(char *block, char typ)
{
    *block |= typ;
}

/* Beschreibung:
 *     bestimmt den Blocktyp einer Koordinate
 * Eingabewerte:
 *     1. Argument: Schicht
 *     2. Argument: x-Wert
 *     3. Argument: y-Wert
 * Rueckgabewerte:
 *     den Blocktyp fuer die Koordinate */
char bestimme_blocktyp(int k, int x, int y)
{
    if (k == 0 && x == 0 && y == 0)
        return (SPITZE);

    if ((x == 0 && y == 0)
        || (x == 0 && y == k)
        || (x == k && y == 0)
        || (x == k && y == k))
        return (ECKSTEIN);

    if (x == 0 || y == 0 || x == k || y == k)
        return (KANTENSTEIN);

    return (MITTELSTEIN);
}
