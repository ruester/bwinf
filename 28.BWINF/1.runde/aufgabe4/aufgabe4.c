/* Bundeswettbewerb Informatik
 * Aufgabe 4 - EU-WAN
 *
 * Autor: Matthias Rüster
 *
 * E-Mail: ruester@molgen.mpg.de 
 */

#include <stdio.h>              /* fuer printf/fopen/getc/... */
#include <stdlib.h>             /* fuer malloc/free/exit/... */
#include <unistd.h>             /* fuer getopt */
#include <math.h>               /* fuer ceil */
#include <string.h>             /* fuer strcmp/strcpy */
#include <limits.h>             /* fuer PATH_MAX */

/* Farben definieren */
#define ROT     255, 0,   0
#define BLAU    58,  95,  205
#define HBLAU   72,  118, 255
#define SCHWARZ 0,   0,   0
#define GRUEN   0,   205, 0

/* Dateiname der Europa-Karte */
#define KARTE "map.pbm"

/* Dateiname des Sendegebietes */
#define SENDER "circle.pbm"

/* Name der Ausgabedateien */
#define AUSGABE "standorte.ppm"

/* zum Finden der Senderstandorte notwendig */
struct Flag {
    char sender,                /* wenn 1, dann steht auf dem Flag ein Sender */
     land,                      /* wenn 1, dann gehoert der Flag zur Landmasse */
     bedeckt;                   /* wenn 1, dann ist das Flag von einem Sendegebiet bedeckt */
};

/* diese Struktur ist zum Suchen der Senderstandorte,
 * zum Speichern der Pixeldaten aus map.pbm und
 * zum Setzen der Sender */
struct Europa {
    struct Flag **flag;         /* 2-dimensionales Flag-Array */
    int hoehe, breite,          /* Hoehe und Breite des Flag-Arrays */
     anzahl_sender;             /* Anzahl der Sender, die gesetzt wurden */
};

/* in dieser Struktur werden die Pixeldaten aus circle.pbm gespeichert */
struct Kreis {
    char **pixel;               /* wenn ein Element des Arrays 1 gesetzt ist,
                                 * dann gehoert der Pixel zum Sendegebiet */
    int radius,                 /* Radius des Sendegebietes */
     hoehe, breite;             /* Hoehe und Breite des Flag-Arrays */
};

/* Beschreibung:
 *     Funktion gibt den Speicher einer Struktur Europa frei
 * Eingabewerte:
 *     eu: Zeiger auf Struktur Europa, deren Speicher
 *         freigegeben werden soll
 * Rueckgabewerte:
 *     keine Rueckgabewerte
 */
void europa_freigeben(struct Europa *eu)
{
    int i;                      /* Zaehlvariable */

    if (eu == NULL)
        return;                 /* Loeschen nicht notwendig */

    /* Zugriffsfehler vermeiden */
    if (eu->flag != NULL) {
        /* Flags freigeben */
        for (i = 0; i < eu->hoehe; i++)
            free(eu->flag[i]);
        free(eu->flag);
    }

    /* Struktur freigeben */
    free(eu);
}

/* Beschreibung:
 *     Funktion gibt den Speicher einer Struktur Kreis frei
 * Eingabewerte:
 *     k: Zeiger auf Struktur Kreis, deren Speicher
 *        freigegeben werden soll 
 * Rueckgabewerte:
 *     keine Rueckgabewerte
 */
void kreis_freigeben(struct Kreis *k)
{
    int i;                      /* Zaehlvariable */

    if (k == NULL)
        return;                 /* Loeschen nicht notwendig */

    /* Zugriffsfehler vermeiden */
    if (k->pixel != NULL) {
        /* Elemente freigeben */
        for (i = 0; i < k->hoehe; i++)
            free(k->pixel[i]);
        free(k->pixel);
    }

    /* Struktur freigeben */
    free(k);
}

/* Beschreibung:
 *     Funktion holt Speicher fuer ein 2-dimensionales char-Array
 * Eingabewerte:
 *     breite: - 'Breite' des Arrays
 *             - muss groesser oder gleich 0 sein
 *     hoehe: - 'Hoehe' des Arrays
 *            - muss groesser oder gleich 0 sein
 * Rueckgabewerte:
 *     - Zeiger auf das char-Array
 *     - bei Fehlern wird das Programm beendet
 */
char **neues_array(int breite, int hoehe)
{
    int i;                      /* Zaehlvariable */
    char **neu;                 /* Zeiger auf Array */

    /* Speicher fuer 'Zeilen' holen */
    if ((neu = calloc(hoehe, sizeof(char *))) == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    /* Speicher fuer 'Spalten' holen */
    for (i = 0; i < hoehe; i++) {
        if ((neu[i] = calloc(breite, sizeof(char))) == NULL) {
            perror("calloc");
            exit(EXIT_FAILURE);
        }
    }

    /* Zeiger zurueckgeben */
    return (neu);
}

/* Beschreibung:
 *     Funktion holt Speicher fuer ein 2-dimensionales Flag-Array
 * Eingabewerte:
 *     breite: - 'Breite' des Arrays
 *             - muss groesser oder gleich 0 sein
 *     hoehe: - 'Hoehe' des Arrays
 *            - muss groesser oder gleich 0 sein
 * Rueckgabewerte:
 *     - Zeiger auf das Flag-Array
 *     - bei Fehlern wird das Programm beendet
 */
struct Flag **neue_flags(int breite, int hoehe)
{
    int i;                      /* Zaehlvariable */
    struct Flag **neu;          /* Zeiger auf Flag-Array */

    /* Speicher fuer "Zeilen" holen */
    if ((neu = calloc(hoehe, sizeof(struct Flag *))) == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    /* Speicher fuer "Spalten" holen */
    for (i = 0; i < hoehe; i++) {
        if ((neu[i] = calloc(breite, sizeof(struct Flag))) == NULL) {
            perror("calloc");
            exit(EXIT_FAILURE);
        }
    }

    /* Zeiger zurueckgeben */
    return (neu);
}

/* Beschreibung:
 *     Funktion holt Speicher fuer eine Struktur Europa und
 *     setzt die land-Flags dieser anhand der Daten einer pbm-Datei
 * Eingabewerte:
 *     karte: der Dateiname der pbm-Datei mit den
 *            Pixeldaten der Europa-Karte
 * Rueckgabewerte:
 *     - Zeiger auf die Struktur Europa
 *     - bei Fehlern wird das Programm beendet
 */
struct Europa *neues_europa_von_pbm(char *karte)
{
    int i, j,                   /* Zaehlvariablen */
     temp;                      /* zum Auslesen der Datei benoetigt */
    FILE *datei;                /* Zeiger auf die Bitmap-Datei */
    char zeile[70];             /* zum Auslesen der Datei benoetigt */
    struct Europa *m;           /* Zeiger auf die Struktur Europa */

    /* Speicher fuer Struktur Europa holen */
    if ((m = calloc(1, sizeof(struct Europa))) == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    /* Datei oeffnen */
    if ((datei = fopen(karte, "r")) == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    /* Bitmap-Typ einlesen */
    if (fgets(zeile, 70, datei) == NULL) {
        printf("Unerwartetes EOF!\n");
        exit(EXIT_FAILURE);
    }

    /* Bitmap-Typ pruefen */
    if (strcmp(zeile, "P1\n") != 0) {
        printf("Die Datei %s ist keine pbm-Datei! %s\n", karte, zeile);
        exit(EXIT_FAILURE);
    }

    /* Kommentar ueberspringen */
    if (fgets(zeile, 70, datei) == NULL
        /* Hoehe und Breite einlesen */
        || fscanf(datei, "%d %d\n", &(m->breite), &(m->hoehe)) == EOF) {
        printf("Unerwartetes EOF!\n");
        exit(EXIT_FAILURE);
    }

    /* Speicher fuer Flag-Array holen */
    m->flag = neue_flags(m->breite, m->hoehe);

    /* Pixeldaten auslesen */
    for (i = 0; i < m->hoehe; i++) {
        for (j = 0; j < m->breite; j++) {
            /* ein Zeichen einlesen */
            temp = fgetc(datei);

            /* die pbm-Datei muss vollstaendig sein */
            if (temp == EOF) {
                printf("Unerwartetes EOF!\n");
                exit(EXIT_FAILURE);
            }

            /* '\n' - Zeichen ueberspringen */
            if (temp == '\n')
                temp = fgetc(datei);

            /* Flag setzen */
            m->flag[i][j].land = temp - '0';
        }
    }

    /* Datei schliessen */
    if (fclose(datei) == EOF) {
        perror("fclose");
        exit(EXIT_FAILURE);
    }

    /* Zeiger zurueckgeben */
    return (m);
}

/* Beschreibung:
 *     Funktion holt Speicher fuer eine Struktur Kreis und setzt
 *     die Flags in der Struktur Kreis anhand der Daten einer Bitmap-Datei
 * Eingabewerte:
 *     pbm: Dateiname der pbm-Datei mit den Pixeldaten
 *          des Sendegebietes
 * Rueckgabewerte:
 *     - Zeiger auf die Struktur Kreis
 *     - bei Fehlern wird das Programm beendet
 */
struct Kreis *neuer_kreis_von_pbm(char *pbm)
{
    FILE *datei;                /* Zeiger auf die Bitmap-Datei */
    char zeile[70];             /* zum Auslesen der Datei benoetigt */
    int i, j,                   /* Zaehlvariablen */
     temp;                      /* zum Auslesen der Datei benoetigt */
    struct Kreis *neu;          /* Zeiger auf die Struktur Kreis */

    /* Speicher fuer die Struktur holen */
    if ((neu = calloc(1, sizeof(struct Kreis))) == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    /* Bitmap-Datei oeffnen */
    if ((datei = fopen(pbm, "r")) == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    /* Bitmap-Typ einlesen */
    if (fgets(zeile, 70, datei) == NULL) {
        perror("fgets");
        exit(EXIT_FAILURE);
    }

    /* Bitmap-Typ pruefen */
    if (strcmp(zeile, "P1\n") != 0) {
        printf("Die Datei %s ist keine pbm-Datei! %s\n", pbm, zeile);
        exit(EXIT_FAILURE);
    }

    /* Hoehe und Breite einlesen */
    if (fscanf(datei, "%d %d\n", &(neu->breite), &(neu->hoehe)) == EOF) {
        printf("Unerwartetes EOF!\n");
        exit(EXIT_FAILURE);
    }

    /* Radius bestimmen */
    neu->radius = (int) ceil((float) neu->breite / 2.0);        /* nicht aufrunden */

    /* Speicher fuer Flag-Array holen */
    neu->pixel = neues_array(neu->breite, neu->hoehe);

    /* Pixeldaten in Struktur Kreis schreiben */
    for (i = 0; i < neu->hoehe; i++) {
        for (j = 0; j < neu->breite; j++) {
            temp = fgetc(datei);        /* Zeichen holen */

            /* die pbm-Datei muss vollstaendig sein */
            if (temp == EOF) {
                printf("Unerwartetes EOF!\n");
                exit(EXIT_FAILURE);
            }

            /* der Pixelwert darf nur 1 oder 0 sein */
            if (temp != '0' && temp != '1') {
                printf("Die Datei %s ist keine pbm-Datei!\n", pbm);
                exit(EXIT_FAILURE);
            }

            /* Pixelwert eintragen */
            neu->pixel[i][j] = temp - '0';
        }
        fgetc(datei);           /* '\n' ueberspringen */
    }

    /* Datei schliessen */
    if (fclose(datei) == EOF) {
        perror("fclose");
        exit(EXIT_FAILURE);
    }

    /* Zeiger zurueckgeben */
    return (neu);
}

/* Beschreibung:
 *     Funktion holt Speicher fuer eine Struktur Europa
 * Eingabewerte:
 *     breite: - 'Breite' des Arrays
 *             - muss groesser oder gleich 0 sein
 *     hoehe: - 'Hoehe' des Arrays
 *            - muss groesser oder gleich 0 sein
 * Rueckgabewerte:
 *     - Zeiger auf die Struktur Europa
 *     - bei Fehlern wird das Programm beendet
 */
struct Europa *neues_europa(int breite, int hoehe)
{
    struct Europa *eu;          /* Zeiger auf Struktur Europa */

    /* Speicher fuer Struktur Europa holen */
    if ((eu = calloc(1, sizeof(struct Europa))) == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    /* Speicher fuer Flag-Array holen */
    eu->flag = neue_flags(breite, hoehe);

    /* Hoehe und Breite des Arrays eintragen */
    eu->hoehe = hoehe;
    eu->breite = breite;

    /* Zeiger zurueckgeben */
    return (eu);
}

/* Beschreibung:
 *     Funktion schreibt die Flags einer Struktur Europa in eine ppm-Datei
 * Eingabewerte:
 *     eu: - Zeiger auf die Struktur Europa mit den gesetzten Flags
 *         - Zeiger darf nicht NULL sein
 *     ausgabe: - Dateiname der Datei, in der das Bild gespeichert wird
 *              - darf nicht NULL sein
 * Rueckgabewerte:
 *     - keine Rueckgabewerte
 *     - bei Fehlern wird das Programm beendet
 */
void zeichne_flags_in_ppm(struct Europa *eu, char *ausgabe)
{
    FILE *datei;                /* Zeiger auf die Ausgabedatei */
    int i, j;                   /* Zaehlvariablen */

    /* Datei zum Schreiben oeffnen */
    if ((datei = fopen(ausgabe, "w")) == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    /* Bitmap-Typ und Hoehe und Breite in Datei schreiben */
    if (fprintf(datei, "P3\n%d %d\n255\n", eu->breite, eu->hoehe) < 0) {
        perror("fprintf");
        exit(EXIT_FAILURE);
    }

    /* Flag-Array durchgehen */
    for (i = 0; i < eu->hoehe; i++) {
        for (j = 0; j < eu->breite; j++) {
            /* auf dem Pixel steht ein Sender */
            if (eu->flag[i][j].sender == 1)
                fprintf(datei, "%d %d %d\n", SCHWARZ);
            /* Landpixel ist von einem Sendegebiet bedeckt */
            else if (eu->flag[i][j].land == 1 && eu->flag[i][j].bedeckt == 1)
                fprintf(datei, "%d %d %d\n", GRUEN);
            /* Landpixel ist nicht von einem Sendegebiet bedeckt */
            else if (eu->flag[i][j].land == 1 && eu->flag[i][j].bedeckt == 0)
                fprintf(datei, "%d %d %d\n", ROT);
            /* Wasserpixel ist von einem Sendegebiet bedeckt */
            else if (eu->flag[i][j].land == 0 && eu->flag[i][j].bedeckt == 1)
                fprintf(datei, "%d %d %d\n", HBLAU);
            /* Wasserpixel ist nicht von einem Sendegebiet bedeckt */
            else if (eu->flag[i][j].land == 0 && eu->flag[i][j].bedeckt == 0)
                fprintf(datei, "%d %d %d\n", BLAU);
        }
    }

    /* Datei schliessen */
    if (fclose(datei) == EOF) {
        perror("fclose");
        exit(EXIT_FAILURE);
    }
}

/* Beschreibung:
 *     Funktion setzt 'sender'- und 'bedeckt'-Flags
 *     in einer Struktur Europa auf den Wert 0
 * Eingabewerte:
 *     eu: Zeiger auf Struktur Europa, in der
 *         die Flags gesetzt werden sollen
 * Rueckgabewerte:
 *     keine Rueckgabewerte
 */
void init_flags(struct Europa *eu)
{
    int i, j;                   /* Zaehlvariablen */

    /* alle Flags durchgehen */
    for (i = 0; i < eu->hoehe; i++) {
        for (j = 0; j < eu->breite; j++) {
            /* 'sender'- und 'bedeckt'-Flags setzen */
            eu->flag[i][j].sender = 0;
            eu->flag[i][j].bedeckt = 0;
        }
    }
}

/* Beschreibung:
 *     Funktion setzt alle 'bedeckt'-Flags, die in einem Sendegebiet von 
 *     einem Sender mit den Koordinaten (x_sender ; y_sender) liegen, auf 1
 * Eingabewerte:
 *     eu: - Zeiger auf die Struktur Europa, in der ein Sender gesetzt wurde
 *         - Zeiger darf nicht NULL sein
 *     k: - Zeiger auf Struktur Kreis, die das Sendegebiet enthaelt
 *        - Zeiger darf nicht NULL sein
 *     x_sender, y_sender: - Koordinaten des platzierten Senders
 *                         - muessen groesser oder gleich 0 sein
 */
void setze_flags_fuer_ein_sender(struct Europa *eu, struct Kreis *k,
                                 int x_sender, int y_sender)
{
    int i, j,                   /* Zaehlvariablen */
     x_eu, y_eu;                /* Koordinaten fuer die Struktur Europa */

    /* Pixeldaten von k in eu schreiben */
    for (i = 0; i < k->hoehe; i++) {
        for (j = 0; j < k->breite; j++) {
            /* Koordinate liegt nicht im Sendegebiet */
            if (k->pixel[i][j] == 0)
                continue;

            /* Koordinaten berechnen */
            x_eu = x_sender - k->radius + j + 1;
            y_eu = y_sender - k->radius + i + 1;

            /* Zugriffsfehler vermeiden */
            if (y_eu >= eu->hoehe)
                return;
            if (x_eu >= eu->breite || y_eu < 0)
                break;
            if (x_eu < 0)
                continue;

            /* Koordinate liegt im Sendegebiet */
            eu->flag[y_eu][x_eu].bedeckt = 1;
        }
    }
}

/* Beschreibung:
 *     Funktion holt Speicher fuer eine Struktur Europa und
 *     kopiert die Werte der 'land'-Flags
 * Eingabewerte:
 *     quelle: - Zeiger auf Struktur Europa, deren land-flags
 *               kopiert werden sollen
 *             - Zeiger darf nicht NULL sein
 * Rueckgabewerte:
 *     Zeiger auf die kopierte Struktur Europa
 */
struct Europa *kopie_erstellen(struct Europa *quelle)
{
    int i, j;                   /* Zaehlvariablen */
    struct Europa *kopie;       /* Zeiger auf Kopie der Struktur */

    kopie = neues_europa(quelle->breite, quelle->hoehe);

    /* land-Flags kopieren */
    for (i = 0; i < quelle->hoehe; i++)
        for (j = 0; j < quelle->breite; j++)
            kopie->flag[i][j].land = quelle->flag[i][j].land;

    return (kopie);
}

/* Beschreibung:
 *     Funktion prueft, ob ganz Europa von Sendegebieten "bedeckt" ist
 * Eingabewerte:
 *     eu: - Zeiger auf Struktur Europa, die geprueft werden soll
 *         - Zeiger darf nicht NULL sein
 * Rueckgabewerte:
 *     - sind alle Landpixel bedeckt wird 1 zurueckgegeben
 *     - ist mindestens ein Landpixel unbedeckt wird 0 zurueckgegeben
 */
int europa_bedeckt(struct Europa *eu)
{
    int i, j;                   /* Zaehlvariablen */

    /* alle Flags durchgehen */
    for (i = 0; i < eu->hoehe; i++)
        for (j = 0; j < eu->breite; j++)
            /* nur das Festland ueberpruefen */
            if (eu->flag[i][j].land == 1)
                /* bedeckt-Flag pruefen */
                if (eu->flag[i][j].bedeckt == 0)
                    return (0); /* Europa wird nicht ganz bedeckt */

    /* alle Flags sind von Sendegebieten bedeckt */
    return (1);
}

/* Beschreibung:
 *     Funktion gibt die Anzahl der Pixel zurueck, die von einem 
 *     Sender neu bedeckt werden (wuerden)
 * Eingabewerte:
 *     eu: - Zeiger auf Struktur Europa in der
 *           ein Sender eventuell platziert werden soll
 *         - Zeiger darf nicht NULL sein
 *     k: - Zeiger auf Struktur mit dem Sendegebiet
 *        - Zeiger darf nicht NULL sein
 *     x_sender, y_sender: - Koordinaten des Senders, der daraufhin
 *                           ueberprueft wird, wie viele land-Pixel
 *                           dieser in 'eu' neu bedecken wuerde
 *                         - muessen groesser oder gleich 0 sein
 * Rueckgabewerte:
 *     - Anzahl der Pixel die der Sender neu bedecken wuerde
 */
int bedeckte_flaeche_von_sender(struct Europa *eu, struct Kreis *k,
                                int x_sender, int y_sender)
{
    struct Europa *temp;        /* temporaere Struktur */
    int i, j,                   /* Zaehlvariablen */
     count;                     /* die Anzahl der bedeckten Pixel */

    /* Speicher fuer temporaere Struktur holen */
    temp = kopie_erstellen(eu);

    /* Sender in temporaere Karte setzen */
    setze_flags_fuer_ein_sender(temp, k, x_sender, y_sender);

    /* Sendegebiet durchgehen */
    for (i = y_sender - k->radius, count = 0; i < y_sender + k->radius; i++) {
        /* Zugriffsfehler vermeiden */
        if (i < 0)
            i = 0;
        if (i >= temp->hoehe) {
            /* Speicher von temporaerer Struktur freigeben */
            europa_freigeben(temp);

            /* Anzahl der neu bedeckten Pixel zurueckgeben */
            return (count);
        }

        for (j = x_sender - k->radius; j < x_sender + k->radius; j++) {
            /* Zugriffsfehler vermeiden */
            if (j < 0)
                j = 0;
            if (j >= temp->breite)
                break;

            /* nur "Pixel" zaehlen, die vom Sendegebiet neu bedeckt wurden */
            if (temp->flag[i][j].land == 1 && temp->flag[i][j].bedeckt == 1
                && eu->flag[i][j].bedeckt == 0)
                count++;        /* "Pixel" wurde neu bedeckt */
        }
    }

    /* Speicher von temporaerer Struktur freigeben */
    europa_freigeben(temp);

    /* Anzahl der neu bedeckten Pixel zurueckgeben */
    return (count);
}

/* Beschreibung:
 *     Funktion sucht nach optimalen Senderstandorten
 * Eingabewerte:
 *     karte: - Dateiname der Datei, die die Europakarte enthaelt
 *            - darf nicht NULL sein
 *     kreis: - Dateiname der Datei, die das Sendegebiet enthaelt
 *            - darf nicht NULL sein
 *     option: - option darf nur die Werte 'l' oder 'w' annehmen
 *             - wenn option 'l' ist wird nur nach Senderstandorten
 *               auf dem Land gesucht
 *             - wenn option 'w' ist wird zusaetzlich nach Senderstandorten
 *               auf dem Wasser gesucht
 * Rueckgabewerte:
 *     - Zeiger auf die Struktur Europa (mit den gesetzten Sendern)
 *     - bei Fehlern wird das Programm beendet
 */
struct Europa *finde_standorte(char *karte, char *kreis, char option)
{
    struct Europa *eu,          /* dort werden die Sender gesetzt */
    *temp;                      /* zum Suchen der Senderstandorte notwendig */
    struct Kreis *kr;           /* das Sendegebiet eines Senders */
    int i, j, k, l,             /* Zaehlvariablen */
     max, b, x_merk, y_merk;    /* zum Suchen der Senderstandorte notwendig */

    /* kr initialisieren und Speicher holen */
    kr = neuer_kreis_von_pbm(kreis);

    /* eu initialisieren und Speicher holen */
    eu = neues_europa_von_pbm(karte);

    /* Speicher holen fuer temp */
    temp = kopie_erstellen(eu);

    /* Flag-Array von temp durchgehen */
    for (i = 0; i < temp->hoehe; i++) {
        for (j = 0; j < temp->breite; j++) {
            /* wenn Landpixel schon bedeckt ist */
            if (eu->flag[i][j].land != 1 || eu->flag[i][j].bedeckt != 0)
                continue;

            /* Pixel ist noch nicht von einem Sendegebiet bedeckt */

            /* setze Sender in temp */
            temp->flag[i][j].sender = 1;

            /* setze bedeckt-Flags fuer den gesetzten Sender */
            setze_flags_fuer_ein_sender(temp, kr, j, i);

            /* initialisieren */
            max = 0;
            x_merk = j;
            y_merk = i;

            /* finde optimalen Senderstandort */
            for (k = i - kr->radius; k < i + kr->radius; k++) {
                /* Zugriffsfehler vermeiden */
                if (k < 0)
                    k = 0;
                if (k >= temp->hoehe)
                    break;

                for (l = j - kr->radius; l < j + kr->radius; l++) {
                    /* Zugriffsfehler vermeiden */
                    if (l < 0)
                        l = 0;
                    if (l >= temp->breite)
                        break;

                    /* je nach option werden Sender nur auf Land
                     * oder auch auf Wasser gesetzt */
                    if ((option == 'l' && temp->flag[k][l].land == 1
                         && temp->flag[k][l].bedeckt == 1)
                        || (option == 'w' && temp->flag[k][l].bedeckt == 1)) {
                        /* neu bedeckte Flaeche von dem 
                         * temporaer gesetzten Sender ermitteln */
                        b = bedeckte_flaeche_von_sender(eu, kr, l, k);

                        /* besten Senderstandort merken */
                        if (b > max) {
                            max = b;
                            x_merk = l;
                            y_merk = k;
                        }
                    }
                }
            }

            /* optimalen Senderstandort eintragen */
            eu->flag[y_merk][x_merk].sender = 1;

            /* 'bedeckt'-Flags fuer den gesetzten Sender setzen */
            setze_flags_fuer_ein_sender(eu, kr, x_merk, y_merk);

            /* Anzahl der platzierten Sender erhoehen */
            eu->anzahl_sender++;

            /* Flags von temp zuruecksetzen */
            init_flags(temp);
        }
    }

    /* Pruefen ob ganz Europa mit Sendegebieten bedeckt ist */
    if (!europa_bedeckt(eu)) {
        printf("Europa ist nicht bedeckt!\n");
        exit(EXIT_FAILURE);
    }

    /* Speicher freigeben */
    europa_freigeben(temp);
    kreis_freigeben(kr);

    /* Adresse der Struktur Europa zurueckgeben */
    return (eu);
}

/* Beschreibung:
 *     Funktion kopiert eine Struktur Europa und
 *     markiert in der Kopie die Senderstandorte mit Kreuzen
 * Eingabewerte:
 *     eu: - Zeiger auf Struktur Europa, die kopiert wird
 *         - Zeiger darf nicht NULL sein
 * Rueckgabewerte:
 *     - Zeiger auf die Struktur Europa, in der
 *       die Senderstandorte als Kreuze dargestellt sind
 *     - bei Fehlern wird das Programm beendet
 */
struct Europa *mit_kreuzen(struct Europa *eu)
{
    int i, j;                   /* Zaehlvariablen */
    struct Europa *kopie;       /* Zeiger auf Struktur Europa */

    /* Speicher holen */
    if ((kopie = neues_europa(eu->breite, eu->hoehe)) == NULL) {
        printf("Konnte keinen Speicher fuer struct Europa holen!\n");
        exit(EXIT_FAILURE);
    }

    /* Flag-Werte kopieren */
    for (i = 0; i < kopie->hoehe; i++)
        for (j = 0; j < kopie->breite; j++)
            kopie->flag[i][j] = eu->flag[i][j];

    /* alle Flags durchgehen */
    for (i = 0; i < eu->hoehe; i++) {
        for (j = 0; j < eu->breite; j++) {
            if (eu->flag[i][j].sender == 1) {
                /* Zugriffsfehler vermeiden */
                if ((i - 1) < 0 && (j - 1) < 0)
                    kopie->flag[i + 1][j + 1].sender = 1;
                else if ((i + 1) >= kopie->hoehe && (j + 1) >= kopie->breite)
                    kopie->flag[i - 1][j - 1].sender = 1;
                else if ((i - 1) < 0 && (j + 1) >= kopie->breite)
                    kopie->flag[i + 1][j - 1].sender = 1;
                else if ((i + 1) >= kopie->hoehe && (j - 1) < 0)
                    kopie->flag[i - 1][j + 1].sender = 1;
                else if ((i - 1) < 0) {
                    kopie->flag[i + 1][j - 1].sender = 1;
                    kopie->flag[i + 1][j + 1].sender = 1;
                } else if ((j - 1) < 0) {
                    kopie->flag[i - 1][j + 1].sender = 1;
                    kopie->flag[i + 1][j + 1].sender = 1;
                } else if ((i + 1) >= kopie->hoehe) {
                    kopie->flag[i - 1][j - 1].sender = 1;
                    kopie->flag[i - 1][j + 1].sender = 1;
                } else if ((j + 1) >= kopie->hoehe) {
                    kopie->flag[i - 1][j - 1].sender = 1;
                    kopie->flag[i + 1][j - 1].sender = 1;
                } else {
                    kopie->flag[i - 1][j - 1].sender = 1;
                    kopie->flag[i - 1][j + 1].sender = 1;
                    kopie->flag[i + 1][j - 1].sender = 1;
                    kopie->flag[i + 1][j + 1].sender = 1;
                }
            }
        }
    }

    /* Zeiger zurueckgeben */
    return (kopie);
}

/* Beschreibung:
 *     Funktion gibt eine Hilfe bezueglich der moeglichen
 *     Parameter des Programms auf den Bildschirm aus
 * Eingabewerte:
 *     keine Eingabewerte benoetigt
 * Rueckgabewerte:
 *     keine Rueckgabewerte
 */
void hilfe_ausgeben()
{
    printf("Benutzung: ./aufgabe4 [Optionen]\n");
    printf("Optionen:\n");
    printf("-k <Dateiname>\tDateiname der Karte\n\n");
    printf("-s <Dateiname>\tDateiname des Sendegebietes\n\n");
    printf("-o <Dateiname>\tDateiname der Ausgabedatei\n\n");
    printf("-l oder -w\tl: Sender nur auf Landflaeche platzieren\n\t");
    printf("\tw: Sender duerfen auch auf Wasserflaeche platziert werden\n\n");
    printf("-h\t\tAusgeben dieser Hilfe\n");
}

/* Beschreibung:
 *      main-Funktion
 * Eingabewerte:
 *     es sind Optionen waehlbar, welche bei
 *     dem Parameter -h erklaert werden
 * Rueckgabewerte:
 *     - beim erfolgreichen Durchlauf des Programms: EXIT_SUCCESS 
 *     - bei Fehlern oder falscher Parameteruebergabe: EXIT_FAILURE
 */
int main(int argc, char *argv[])
{
    int c;                      /* zum Auswerten der Parameter benoetigt */
    char option;                /* Art der Suche */
    char karte[PATH_MAX],       /* Dateiname der Karte */
     sender[PATH_MAX],          /* Dateiname des Sendegebietes */
     ausgabe[PATH_MAX];         /* Dateiname der Ausgabedatei */
    struct Europa *eu,          /* fuer Senderstandorte */
    *kopie;                     /* fuer die Darstellung der
                                 * Senderstandorte als Kreuze */

    /* Standard-Werte festlegen */
    strcpy(karte, KARTE);
    strcpy(sender, SENDER);
    strcpy(ausgabe, AUSGABE);
    option = 'l';

    /* Auswerten der Parameter */
    while ((c = getopt(argc, argv, "k:s:o:lwh")) != -1) {
        switch (c) {
            /* Dateiname des Bildes der Karte */
        case 'k':
            strcpy(karte, optarg);
            break;

            /* Dateiname des Bildes des Sendegebietes */
        case 's':
            strcpy(sender, optarg);
            break;

            /* Ausgabe-Dateiname */
        case 'o':
            strcpy(ausgabe, optarg);
            break;

            /* Hilfe */
        case 'h':
            hilfe_ausgeben();
            return (EXIT_SUCCESS);
            break;

            /* Art der Suche */
        case 'l':              /* nur Landflaeche */
        case 'w':              /* mit Wasserflaeche */
            option = c;
            break;

            /* Falsche Parametereingabe */
        case '?':
            if (optopt == 'k' || optopt == 's' || optopt == 'o')
                printf("Option -%c benoetigt ein Argument!\n", optopt);
            else
                printf("Unbekannte Option %c!\n", optopt);

            return (EXIT_FAILURE);
            break;
        }
    }

    /* Art der Suche ausgeben */
    printf("Starte Suche fuer Senderstandorte");
    if (option == 'l')
        printf(" (nur Landflaeche)\n");
    else
        printf(" (auch Wasserflaeche)\n");

    /* Senderstandorte suchen */
    eu = finde_standorte(karte, sender, option);

    /* Benutzer benachrichtigen */
    printf("Senderstandorte wurden in %s eingezeichnet\n", ausgabe);
    printf("Anzahl Sender: %d\n", eu->anzahl_sender);

    /* Senderstandorte mit Kreuzen versehen */
    kopie = mit_kreuzen(eu);

    /* Ausgabe der Senderstandorte in eine ppm-Datei */
    zeichne_flags_in_ppm(kopie, ausgabe);

    /* Speicher freigeben */
    europa_freigeben(eu);
    europa_freigeben(kopie);

    return (EXIT_SUCCESS);
}
