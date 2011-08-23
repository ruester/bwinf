/* Bundeswettbewerb Informatik
 * Aufgabe 5 - Teleskop
 *
 * Autor: Matthias Rüster
 *
 * E-Mail: ruester@molgen.mpg.de 
 */

#include <stdio.h>              /* fuer printf/fprintf/... */
#include <stdlib.h>             /* fuer calloc/free/... */
#include <string.h>             /* fuer strlen/strcpy */
#include <math.h>               /* fuer ceil/floor */
#include <limits.h>             /* fuer PATH_MAX */
#include <unistd.h>             /* fuer getopt */

/* je hoeher die Toleranz ist,
 * desto mehr Pixel gehoeren NICHT zum Fleck */
#define TOLERANZ 13

/* um die Laufzeit zu verringern:
 *  MIN gibt an wie viele Pixel mindestens 
 *  aneinander haengen muessen um zum Fleck zu gehoeren */
#define MIN 20

/* Dateinamen fuer Ausgabedateien */
#define FLECK "fleck.ppm"
#define KORRIGIERT "korrigiert.ppm"

/* Dateiname des hoch aufgeloesten Bildes */
#define HOCH "bild1-fleck.ppm"

/* Dateiname des niedrig aufgeloesten Bildes */
#define NIEDRIG "bild1-klein.ppm"

/* maximaler RGB-Wert */
#define MAX_RGB 255

/* Farbwerte */
#define BLAU  0,   0,   255
#define WEISS 255, 255, 255

/* Struktur fuer ein Pixel */
struct pixel {
    /* RGB - Werte */
    unsigned char r,            /* rot - Anteil */
     g,                         /* gruen - Anteil */
     b;                         /* blau - Anteil */
};

/* Struktur fuer eine ppm-Datei */
struct ppm {
    char dateiname[PATH_MAX];   /* Dateiname des Bildes */
    char identifikation[3];     /* Bitmap-Typ */
    int hoehe, breite;          /* Hoehe und Breite des Bildes */
    struct pixel **pixel;       /* Pixeldaten */
};

/* Beschreibung:
 *     Funktion gibt den Speicher einer Struktur ppm frei
 * Eingabewerte:
 *     ppm: Zeiger auf Struktur ppm, deren Speicher
 *          freigegeben werden soll
 * Rueckgabewerte:
 *     keine Rueckgabewerte
 */
void ppm_freigeben(struct ppm *ppm)
{
    int i;                      /* Zaehlvariable */

    if (ppm == NULL)
        return;                 /* Loeschen nicht notwendig */

    /* Zugriffsfehler vermeiden */
    if (ppm->pixel != NULL) {
        /* Pixel freigeben */
        for (i = 0; i < ppm->hoehe; i++)
            free(ppm->pixel[i]);
        free(ppm->pixel);
    }

    /* Struktur freigeben */
    free(ppm);
}

/* Beschreibung:
 *     Funktion holt Speicher fuer eine Struktur ppm
 * Eingabewerte:
 *     keine Eingabewerte benoetigt
 * Rueckgabewerte:
 *     - Zeiger auf Struktur ppm
 *     - bei Fehlern wird das Programm beendet
 */
struct ppm *neues_ppm()
{
    struct ppm *neu;            /* Zeiger auf Struktur */

    /* Speicher fuer Struktur holen */
    if ((neu = calloc(1, sizeof(struct ppm))) == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    /* Adresse der Struktur zurueckgeben */
    return (neu);
}

/* Beschreibung:
 *     Funktion setzt die Eigenschaften einer Struktur ppm
 * Eingabewerte:
 *     ppm: - Zeiger auf Struktur ppm, deren Eigenschaften
 *            gesetzt werden sollen
 *          - Zeiger darf nicht NULL sein
 *     dateiname: - Dateiname der ppm-Datei
 *                - darf nicht NULL sein
 *     identifikation: - Bitmap-Typ
 *                     - darf nicht NULL sein
 *     hoehe, breite: - Hoehe und Breite des ppm-Bildes
 *                    - muessen groesser oder gleich 0 sein
 * Rueckgabewerte:
 *     keine Rueckgabewerte
 */
void setze_eigenschaften(struct ppm *ppm, char *dateiname,
                         char *identifikation, int breite, int hoehe)
{
    /* Dateinamen eintragen */
    strcpy(ppm->dateiname, dateiname);

    /* Bitmap-Identifikation eintragen */
    strcpy(ppm->identifikation, identifikation);

    /* Hoehe und Breite des Pixel-Arrays eintragen */
    ppm->breite = breite;
    ppm->hoehe = hoehe;
}

/* Beschreibung:
 *     Funktion holt Speicher fuer ein Pixel-Array
 * Eingabewerte:
 *     breite, hoehe: - 'Hoehe' und 'Breite' des Arrays
 *                    - muessen groesser oder gleich 0 sein
 * Rueckgabewerte:
 *     - Zeiger auf Array
 *     - bei Fehlern wird das Programm beendet
 */
struct pixel **neues_pixelarray(int breite, int hoehe)
{
    struct pixel **array;       /* Zeiger auf Pixel-Array */
    int i;                      /* Zaehlvariable */

    /* Speicher fuer "Zeilen" holen */
    if ((array = calloc(hoehe, sizeof(struct pixel *))) == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    /* Speicher fuer "Spalten" holen */
    for (i = 0; i < hoehe; i++) {
        if ((array[i] = calloc(breite, sizeof(struct pixel))) == NULL) {
            perror("calloc");
            exit(EXIT_FAILURE);
        }
    }

    /* Adresse des Arrays zurueckgeben */
    return (array);
}

/* Beschreibung:
 *     schreibt die Meta- und Pixeldaten eines ppm-Bildes in eine Struktur ppm
 *     und holt dabei Heap-Speicher fuer das Pixel-Array
 * Eingabewerte:
 *     ppm: - Zeiger auf Struktur ppm, die initialisiert werden soll
 *          - Zeiger darf nicht NULL sein
 *     dateiname: - Dateiname des ppm-Bildes
 *                - darf nicht NULL sein
 * Rueckgabewerte:
 *     - keine Rueckgabewerte
 *     - bei Fehlern wird das Programm beendet
 */
void init_ppm(struct ppm *ppm, char *dateiname)
{
    int i, j;                   /* Zaehlvariablen */
    FILE *datei;                /* Zeiger auf Datei */
    char zeile[80];             /* zum Auslesen einer Zeile */

    /* Datei oeffnen */
    if ((datei = fopen(dateiname, "r")) == NULL) {
        perror("fclose");
        exit(EXIT_FAILURE);
    }

    /* Dateinamen eintragen */
    strcpy(ppm->dateiname, dateiname);

    /* ppm-Typ einlesen */
    if (fscanf(datei, "%s\n", ppm->identifikation) == EOF) {
        printf("Unerwartetes Dateiende!\n");
        exit(EXIT_FAILURE);
    }

    /* eventuelle Kommentare ueberspringen */
    do {
        if (fgets(zeile, 80, datei) == NULL) {
            printf("Unerwartetes Dateiende!\n");
            exit(EXIT_FAILURE);
        }
    }
    while (zeile[0] == '#');

    /* Hoehe und Breite des Bildes einlesen */
    sscanf(zeile, "%d %d\n", &(ppm->breite), &(ppm->hoehe));

    /* RGB-Maximalwert ueberspringen */
    if (fgets(zeile, 80, datei) == NULL) {
        printf("Unerwartetes Dateiende!\n");
        exit(EXIT_FAILURE);
    }

    /* Speicher fuer Pixel-Array holen */
    ppm->pixel = neues_pixelarray(ppm->breite, ppm->hoehe);

    /* Pixeldaten einlesen */
    for (i = 0; i < ppm->hoehe; i++) {
        for (j = 0; j < ppm->breite; j++) {
            /* RGB-Werte fuer ein Pixel auslesen */
            if (fscanf(datei, "%c", &(ppm->pixel[i][j].r)) == EOF
                || fscanf(datei, "%c", &(ppm->pixel[i][j].g)) == EOF
                || fscanf(datei, "%c", &(ppm->pixel[i][j].b)) == EOF) {
                printf("Unerwartetes Dateiende!\n");
                exit(EXIT_FAILURE);
            }
        }
    }

    /* Datei schliessen */
    if (fclose(datei) == EOF) {
        perror("fclose");
        exit(EXIT_FAILURE);;
    }
}

/* Beschreibung:
 *     Funktion schreibt eine Struktur ppm in eine Datei (als ppm-Bild)
 * Eingabewerte:
 *     ppm: - Zeiger auf Struktur ppm, die als Bild abgespeichert wird
 *          - Zeiger darf nicht NULL sein
 *     dateiname: - Dateiname der Datei, in die das ppm-Bild abgespeichert wird
 * Rueckgabewerte:
 *     - keine Rueckgabewerte
 *     - bei Fehlern wird das Programm beendet
 */
void schreibe_ppm_in_datei(struct ppm *ppm, char *dateiname)
{
    int i, j;                   /* Zaehlvariablen */
    FILE *datei;                /* Zeiger auf Datei */

    /* Datei oeffnen */
    if ((datei = fopen(dateiname, "w")) == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);;
    }

    /* Dateiname in Struktur eintragen */
    strcpy(ppm->dateiname, dateiname);

    /* Eigenschaften in Datei schreiben */
    fprintf(datei, "%s\n", ppm->identifikation);
    fprintf(datei, "%d %d\n", ppm->breite, ppm->hoehe);
    fprintf(datei, "%hd\n", MAX_RGB);

    /* Pixeldaten in Datei schreiben */
    for (i = 0; i < ppm->hoehe; i++) {
        for (j = 0; j < ppm->breite; j++) {
            fprintf(datei, "%c", ppm->pixel[i][j].r);
            fprintf(datei, "%c", ppm->pixel[i][j].g);
            fprintf(datei, "%c", ppm->pixel[i][j].b);
        }
    }

    /* Datei schreiben */
    fclose(datei);
}

/* Beschreibung:
 * Funktion bestimmt das Aufloesungsverhaeltnis zwischen
 * Hoehe und Breite von zwei ppm-Bildern
 * Eingabewerte:
 *     h: - Zeiger auf Struktur ppm, des hoch aufgeloesten Bildes
 *        - Zeiger darf nicht NULL sein
 *     l: - Zeiger auf Struktur ppm, des niedrig aufgeloesten Bildes
 *        - Zeiger darf nicht NULL sein
 * Rueckgabewerte:
 *     - das Aufloesungsverhaeltnis der Bilder
 *     - wenn Aufloesungsverhaeltnis nicht bestimmbar: -1
 */
int aufloesungsverhaeltnis(struct ppm *h, struct ppm *l)
{
    int relation1, relation2;

    relation1 = (int) floor((float) h->hoehe / (float) l->hoehe);
    relation2 = (int) floor((float) h->breite / (float) l->breite);
    if (relation1 == relation2)
        return (relation1);

    relation1 = (int) ceil((float) h->hoehe / (float) l->hoehe);
    relation2 = (int) ceil((float) h->breite / (float) l->breite);
    if (relation1 == relation2)
        return (relation1);

    relation1 = (int) floor((float) h->hoehe / (float) l->hoehe);
    relation2 = (int) ceil((float) h->breite / (float) l->breite);
    if (relation1 == relation2)
        return (relation1);

    relation1 = (int) ceil((float) h->hoehe / (float) l->hoehe);
    relation2 = (int) floor((float) h->breite / (float) l->breite);
    if (relation1 == relation2)
        return (relation1);

    return (-1);
}

/* Beschreibung:
 *     Funktion bestimmt die Anzahl der zusammenhaengenden Pixel
 *     an einer Stelle (x ; y) in einer ppm-Struktur, dabei
 *     wird entweder in x- oder y-Richtung auf anliegende Pixel geprueft
 * Eingabewerte:
 *     ppm: - Zeiger auf Struktur ppm, in der nach
 *            anliegenden Pixeln gesucht wird
 *          - Zeiger darf nicht NULL sein
 *     x, y: - Koordinaten der Stelle
 *     richtung: - richtung darf nur die Werte 'x' oder 'y' annehmen
 *               - wenn richtung 'x' ist, dann wird in x-Richtung geprueft
 *               - wenn richtung 'y' ist, dann wird in y-Richtung geprueft
 * Rueckgabewerte:
 *     - Anzahl der zusammenhaengenden Pixel an der
 *       Stelle x;y in Richtung x bzw. y
 */
int anliegende_pixel(struct ppm *ppm, int x, int y, char richtung)
{
    int i, count;               /* Zaehlvariablen */

    count = 0;

    switch (richtung) {
    case 'y':
        /* im Pixel-Array "nach unten" gehen */
        for (i = 0; y + i < ppm->hoehe && count <= MIN; i++)
            if (ppm->pixel[y + i][x].r == 0)
                count++;
            else
                break;

        /* im Pixel-Array "nach oben" gehen */
        for (i = 1; y - i >= 0 && count <= MIN; i++)
            if (ppm->pixel[y - i][x].r == 0)
                count++;
            else
                break;
        break;

    case 'x':
        /* im Pixel-Array "nach rechts" gehen */
        for (i = 0; x + i < ppm->breite && count <= MIN; i++)
            if (ppm->pixel[y][x + i].r == 0)
                count++;
            else
                break;

        /* im Pixel-Array "nach links" gehen */
        for (i = 1; x - i >= 0 && count <= MIN; i++)
            if (ppm->pixel[y][x - i].r == 0)
                count++;
            else
                break;
        break;
    }

    /* Anzahl der anliegenden Pixel zurueckgeben */
    return (count);
}

/* Beschreibung:
 *     Funktion setzt die RGB-Werte eines Pixels
 * Eingabewerte:
 *     p: Zeiger auf Struktur pixel, deren Werte gesetzt werden
 *     r, g, b: RGB-Farbwert
 * Rueckgabewerte:
 *     keine Rueckgabewerte
 */
void set_pixel(struct pixel *p,
               unsigned char r, unsigned char g, unsigned char b)
{
    p->r = r;
    p->g = g;
    p->b = b;
}

/* Beschreibung:
 *     Funktion 'findet' einen groesseren Bereich von Pixeln und 'loescht'
 *     alle anderen Pixel die nicht zu diesem Bereich gehoeren
 * Eingabewerte:
 *     ppm: - Zeiger auf Struktur ppm, wo nach dem Bereich gesucht wird
 *          - Zeiger darf nicht NULL sein
 * Rueckgabewerte:
 *    keine Rueckgabewerte
 */
void nur_bereich(struct ppm *ppm)
{
    int i, j,                   /* Zaehlvariablen */
     count_x, count_y,          /*  */
     merk;                      /*  */

    for (i = 0; i < ppm->hoehe; i++) {
        for (j = 0, merk = 0; j < ppm->breite; j++) {
            if (ppm->pixel[i][j].r == 255)
                continue;
            count_x = anliegende_pixel(ppm, j, i, 'x');
            count_y = anliegende_pixel(ppm, j, i, 'y');
            if (count_x < MIN || count_y < MIN) {
                set_pixel(&(ppm->pixel[i][j]), WEISS);
                merk = 1;
            }
        }

        /* wenn Pixelwerte veraendert wurden */
        if (merk)
            i--;                /* nochmal die Pixel davor pruefen */
    }
}

/* Beschreibung:
 *     Funktion vergleicht die Pixelwerte zweier Pixel
 * Eingabewerte:
 *     p, q: - Zeiger auf Pixel, die verglichen werden
 *           - Zeiger duerfen nicht NULL sein
 *     toleranz: - die Toleranz gibt an, wie gross die Differenz
 *                 zwischen den RGB-Werten der Pixel sein darf
 *                 um als 'aehnlich' erkannt zu werden
 *               - darf nicht kleiner 0 sein
 * Rueckgabewerte:
 *     - wenn Pixel aehnliche Werte: 1
 *     - wenn Pixel nicht aehnliche Pixel haben: 0
 */
int vergleiche_pixel(struct pixel *p, struct pixel *q, int toleranz)
{
    return ((p->r + toleranz >= q->r && p->r - toleranz <= q->r)
            && (p->g + toleranz >= q->g && p->g - toleranz <= q->g)
            && (p->b + toleranz >= q->b && p->b - toleranz <= q->b));
}

/* Beschreibung:
 *     Funktion findet einen fehlerhaften Bereich in einem ppm-Bild
 *     und schreibt diesen in eine ppm-Struktur (der fehlerhafte Bereich
 *     wird blau dargestellt)
 * Eingabewerte:
 *     hoch: - Zeiger auf Struktur ppm des hoch aufgeloesten Bildes
 *           - Zeiger darf nicht NULL sein
 *     niedrig: - Zeiger auf Struktur ppm des niedrig aufgeloesten Bildes
 *              - Zeiger darf nicht NULL sein
 * Rueckgabewerte:
 *     Zeiger auf Struktur ppm mit dem fehlerhaften Bereich
 */
struct ppm *fehlerhafter_bereich(struct ppm *hoch, struct ppm *niedrig)
{
    struct ppm *f;              /* Zeiger auf Struktur ppm */
    int i, j, m, n,             /* Zaehlvariablen */
     x, y,                      /* Koordinaten */
     rel;                       /* Aufloesungsverhaeltnis zwischen dem
                                 * hoch und dem niedrig aufgeloesten Bild */

    /* Speicher fuer Struktur holen */
    f = neues_ppm();

    /* Eigenschaften des hoch aufgeloesten Bildes uebernehmen */
    setze_eigenschaften(f, "", hoch->identifikation,
                        hoch->breite, hoch->hoehe);

    /* Speicher fuer Pixel-Array holen */
    f->pixel = neues_pixelarray(hoch->breite, hoch->hoehe);

    /* Aufloesungsverhaeltnis ermitteln */
    rel = aufloesungsverhaeltnis(hoch, niedrig);

    /* alle Pixel durchgehen */
    for (i = 0; i < niedrig->hoehe; i++) {
        for (j = 0; j < niedrig->breite; j++) {
            /* Aufloesungsverhaeltnis beachten */
            for (m = 0; m < rel; m++) {
                for (n = 0; n < rel; n++) {
                    /* Berechnen der Koordinaten auf
                     * dem hoch aufgeloesten Bild */
                    x = j * rel + n;
                    y = i * rel + m;

                    /* Zugriffsfehler vermeiden */
                    if (x >= f->breite || y >= f->hoehe)
                        break;

                    /* Vergleich des Pixels vom hoch aufgeloesten Bild
                     * mit dem des niedrig aufgeloesten Bildes */
                    if (vergleiche_pixel(&(niedrig->pixel[i][j]),
                                         &(hoch->pixel[y][x]), TOLERANZ) == 0)
                        /* Pixel ist eventuell fehlerhaft */
                        set_pixel(&(f->pixel[y][x]), BLAU);
                    else
                        /* Pixel ist nicht fehlerhaft */
                        set_pixel(&(f->pixel[y][x]), WEISS);
                }
            }
        }
    }

    /* fehlerhaften Bereich ermitteln */
    nur_bereich(f);

    /* Adresse der Struktur zurueckgeben */
    return (f);
}

/* Beschreibung:
 *     Funktion korrigiert den fehlerhaften Bereich des
 *     hoch aufgeloesten Bildes mit Hilfe des niedrig aufgeloesten Bildes
 * Eingabewerte:
 *     hoch: - Zeiger auf Struktur ppm des hoch aufgeloesten Bildes
 *           - Zeiger darf nicht NULL sein
 *     niedrig: - Zeiger auf Struktur ppm des niedrig aufgeloesten Bildes
 *              - Zeiger darf nicht NULL sein
 *     fehler: - Zeiger auf Struktur ppm des fehlerhaften Bereichs
 *             - Zeiger darf nicht NULL sein
 * Rueckgabewerte:
 *     Zeiger auf Struktur ppm mit dem korrigierten Bild
 */
struct ppm *korrigiere(struct ppm *hoch, struct ppm *niedrig,
                       struct ppm *fehler)
{
    struct ppm *k;              /* Zeiger auf Struktur ppm */
    int i, j, m, n,             /* Zaehlvariablen */
     x, y,                      /* Koordinaten */
     rel;                       /* Aufloesungsverhaeltnis zwischen dem hoch
                                 * und dem niedrig aufgeloesten Bild */

    /* Speicher holen */
    k = neues_ppm();

    /* Eigenschaften des hoch aufgeloesten Bildes uebernehmen */
    setze_eigenschaften(k, "", hoch->identifikation,
                        hoch->breite, hoch->hoehe);

    /* Speicher fuer Pixel-Array holen */
    k->pixel = neues_pixelarray(hoch->breite, hoch->hoehe);

    /* Verhaeltnis zwischen den Aufloesungen ermitteln */
    rel = aufloesungsverhaeltnis(hoch, niedrig);

    /* alle Pixel durchgehen */
    for (i = 0; i < niedrig->hoehe; i++) {
        for (j = 0; j < niedrig->breite; j++) {
            /* Aufloesungsverhaeltnis beachten */
            for (m = 0; m < rel; m++) {
                for (n = 0; n < rel; n++) {
                    /* Berechnen der Koordinaten auf
                     * dem hoch aufgeloesten Bild */
                    x = j * rel + n;
                    y = i * rel + m;

                    /* Zugriffsfehler vermeiden */
                    if (x >= k->breite || y >= k->hoehe)
                        break;

                    /* wenn Pixel blau ist */
                    if (fehler->pixel[y][x].r == 0
                        && fehler->pixel[y][x].g == 0)
                        /* blauen Pixel mit Pixel des 
                         * niedrig aufgeloesten Bildes ersetzten */
                        set_pixel(&(k->pixel[y][x]), niedrig->pixel[i][j].r,
                                  niedrig->pixel[i][j].g,
                                  niedrig->pixel[i][j].b);
                    else        /* weissen Pixel mit Pixel des
                                 * hoch aufgeloesten Bildes ersetzten */
                        set_pixel(&(k->pixel[y][x]), hoch->pixel[y][x].r,
                                  hoch->pixel[y][x].g, hoch->pixel[y][x].b);
                }
            }
        }
    }

    /* Adresse der Struktur zurueckgeben */
    return (k);
}

/* Beschreibung:
 *     Funktion ersetzt den fehlerhaften Bereich
 *     durch Originalpixel aus dem hoch aufgeloesten Bild
 * Eingabewerte:
 *     blau: - Zeiger auf Struktur ppm mit dem fehlerhaften Bereich
 *           - Zeiger darf nicht NULL sein
 *     hoch: - Zeiger auf Struktur ppm des hoch aufgeloesten Bildes
 *           - Zeiger darf nicht NULL sein
 * Rueckgabewerte:
 *     Zeiger auf Struktur ppm mit dem korrigierten Bild
 */
struct ppm *ersetze_blauen_bereich(struct ppm *blau, struct ppm *hoch)
{
    int i, j;                   /* Zaehlvariablen */
    struct ppm *k;              /* Zeiger auf ppm Struktur */

    /* Speicher holen */
    k = neues_ppm();

    /* Eigenschaften des hoch aufgeloesten Bildes uebernehmen */
    setze_eigenschaften(k, "", hoch->identifikation,
                        hoch->breite, hoch->hoehe);

    /* Speicher fuer das Pixel-Array holen */
    k->pixel = neues_pixelarray(hoch->breite, hoch->hoehe);

    /* alle Pixel durchgehen */
    for (i = 0; i < k->hoehe; i++) {
        for (j = 0; j < k->breite; j++) {
            if (blau->pixel[i][j].r == 0 && blau->pixel[i][j].g == 0)
                /* blauen Pixel mit dem Originalpixel vom
                 * hoch aufgeloesten Bild ersetzen */
                set_pixel(&(k->pixel[i][j]), hoch->pixel[i][j].r,
                          hoch->pixel[i][j].g, hoch->pixel[i][j].b);
            else
                /* weissen Pixel als Hintergrundfarbe setzten */
                set_pixel(&(k->pixel[i][j]), WEISS);
        }
    }

    /* Adresse der Struktur zurueckgeben */
    return (k);
}

/* Beschreibung:
 *     Funktion gibt eine Hilfe bezueglich der moeglichen Optionen
 *     des Programm auf den Bildschirm aus
 * Eingabewerte:
 *     keine Eingabewerte benoetigt
 * Rueckgabewerte:
 *     keine Rueckgabewerte
 */
void hilfe_ausgeben()
{
    printf("Benutzung: ./aufgabe5 [Optionen]\n");
    printf("Optionen:\n");
    printf("-f <Dateiname>\tDateiname des hoch aufgeloesten");
    printf(" Bildes mit dem Fleck\n\n");
    printf("-n <Dateiname>\tDateiname des niedrig aufgeloesten Bildes\n\n");
    printf("-o <Dateiname>\tAusgabe: Dateiname des Bildes mit dem Fleck\n\n");
    printf("-k <Dateiname>\tAusgabe: Dateiname des korrigierten Bildes\n\n");
    printf("-h\t\tAusgeben dieser Hilfe\n");
}

/* Beschreibung:
 *     main-Funktion
 * Eingabewerte:
 *     argv: moegliche Optionen werden mit dem Parameter -h ausgegeben
 * Rueckgabewerte:
 *     - beim erfolgreichen Durchlauf des Programms: EXIT_SUCCESS 
 *     - bei Fehlern oder falscher Parameteruebergabe: EXIT_FAILURE
 */
int main(int argc, char *argv[])
{
    int c;                      /* zum Auswerten der Parameter benoetigt */
    struct ppm *fleck,          /* hoch aufgeloestes Bild (mit dem "Fleck") */
    *klein,                     /* niedrig aufgeloestes Bild */
    *fehlerhaft,                /* fehlerhafter Bereich mit "originalen" Pixeln */
    *bereich,                   /* fehlerhafter Bereich mit blauen Pixeln */
    *korrigiert;                /* korrigiertes Bild */
    char hoch[PATH_MAX],        /* Dateiname des hoch aufgeloesten Bildes */
     niedrig[PATH_MAX],         /* Dateiname des niedrig aufgeloesten Bildes */
     fleck_ausgabe[PATH_MAX],   /* Dateiname des Bildes mit dem Fleck */
     korrigiert_ausgabe[PATH_MAX];      /* Dateiname des korrigierten Bildes */

    /* Standard-Werte festlegen */
    strcpy(hoch, HOCH);
    strcpy(niedrig, NIEDRIG);
    strcpy(fleck_ausgabe, FLECK);
    strcpy(korrigiert_ausgabe, KORRIGIERT);

    /* Auswerten der Parameter */
    while ((c = getopt(argc, argv, "f:n:o:k:h")) != -1) {
        switch (c) {
            /* Dateiname des hoch aufgeloesten Bildes */
        case 'f':
            strcpy(hoch, optarg);
            break;

            /* Dateiname des niedrig aufgeloesten Bildes */
        case 'n':
            strcpy(niedrig, optarg);
            break;

            /* Ausgabe-Dateiname des Fleckes */
        case 'o':
            strcpy(fleck_ausgabe, optarg);
            break;

            /* Ausgabe-Dateiname des korrigierten Bildes */
        case 'k':
            strcpy(korrigiert_ausgabe, optarg);
            break;

            /* Hilfe */
        case 'h':
            hilfe_ausgeben();
            return (EXIT_SUCCESS);
            break;

            /* Fehler */
        case '?':
            if (optopt == 'f' || optopt == 'n')
                printf("Option -%c benoetigt ein Argument!\n", optopt);
            else
                printf("Unbekannte Option %c!\n", optopt);

            return (EXIT_FAILURE);
            break;
        }
    }

    /* Speicher holen */
    fleck = neues_ppm();
    klein = neues_ppm();

    /* Bilddaten in Strukturen schreiben */
    init_ppm(fleck, hoch);
    init_ppm(klein, niedrig);

    /* fehlerhaften Bereich auf dem hoch aufgeloesten Bild herausfinden */
    bereich = fehlerhafter_bereich(fleck, klein);

    /* gefundenen fehlerhaften Bereich mit Originalpixeln
     * des hoch aufgeloesten Bildes ersetzten */
    fehlerhaft = ersetze_blauen_bereich(bereich, fleck);

    /* den "Fleck" in einem Bild abspeichern */
    schreibe_ppm_in_datei(fehlerhaft, fleck_ausgabe);

    /* Korrigieren des fehlerhaften Bereichs
     * mit Hilfe des niedrig aufgeloesten Bildes */
    korrigiert = korrigiere(fleck, klein, bereich);

    /* korrigiertes Bild abspeichern */
    schreibe_ppm_in_datei(korrigiert, korrigiert_ausgabe);

    /* Benutzer benachrichtigen */
    printf("Der fehlerhafte Bereich im Bild %s", fleck->dateiname);
    printf(" wurde mit dem Bild %s korrigiert.\n", klein->dateiname);
    printf("Der fehlerhafte Bereich wurde in");
    printf(" der Datei %s abgespeichert.\n", fehlerhaft->dateiname);
    printf("Das korrigierte Bild wurde in");
    printf(" der Datei %s abgespeichert.\n", korrigiert->dateiname);

    /* Speicher freigeben */
    ppm_freigeben(korrigiert);
    ppm_freigeben(bereich);
    ppm_freigeben(fehlerhaft);
    ppm_freigeben(fleck);
    ppm_freigeben(klein);

    return (EXIT_SUCCESS);
}
