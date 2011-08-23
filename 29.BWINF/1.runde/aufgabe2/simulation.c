/* Bundeswettbewerb Informatik
 * Aufgabe 2 - Robuttons
 *
 * Autor: Matthias Rüster
 *
 * E-Mail: ruester@molgen.mpg.de
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <limits.h>

#include <SDL.h>

#include "simulation.h"
#include "kollision.h"
#include "robutton.h"
#include "tisch.h"
#include "vektor.h"

/* Farben */
#define BLAU  0,   0,   255
#define GRUEN 69,  139, 0
#define ROT   255, 0,   0
#define CYAN  0,   255, 255
#define BRAUN 139, 121, 94
#define GELB  255, 246, 143

/* Beschreibung:
 *     setzt einen Pixel in einer SDL_Surface
 * Eingabewerte:
 *     1. Argument: Zeiger auf die SDL_Surface
 *     2. Argument: x-Wert der Pixelkoordinate
 *     3. Argument: y-Wert der Pixelkoordinate
 *     4. Argument: RGB-Wert
 * Rueckgabewerte:
 *     keine */
void setze_pixel(SDL_Surface *surface, int x, int y, int pixel)
{
    int *target_pixel;
    target_pixel = (int *) surface->pixels + y * surface->w + x;
    *target_pixel = pixel;
}

/* Beschreibung:
 *     bestimmt die Koordinaten, die zur Tischflaeche gehoeren
 * Eingabewerte:
 *     Zeiger auf die Simulationsstruktur
 * Rueckgabewerte:
 *     keine */
void bestimme_tischflaeche(struct simulation *s)
{
    int x, y;

    for (y = 0; y < F_HOEHE; y++)
        for (x = 0; x < F_BREITE; x++)
            if (ist_tischflaeche(s->tisch, x, y))
                s->tischflaeche[y][x] = 1;
            else
                s->tischflaeche[y][x] = 0;
}

/* Beschreibung:
 *     prueft, ob an der Stelle x;y schon ein Robutton platziert ist
 * Eingabewerte:
 *     1. Argument: Zeiger auf die Simulationsstruktur
 *     2. Argument: x-Wert der Koordinate
 *     3. Argument: y-Wert der Koordinate
 * Rueckgabewerte:
 *     0, wenn dort kein Robutton platziert ist
 *     1, wenn dort ein Robutton platziert ist */
char robutton_platziert(struct simulation *s, int x, int y)
{
    int i;

    for (i = 0; i < s->anzahl_robuttons; i++)
        if (x == (int) s->robuttons[i].position.x
            && y == (int) s->robuttons[i].position.y)
            return (1);

    return (0);
}

/* Beschreibung:
 *     prueft, ob an der Stelle x;y schon eine Muenze platziert ist
 * Eingabewerte:
 *     1. Argument: Zeiger auf die Simulationsstruktur
 *     2 und 3. Argument: zu ueberpruefende Koordinate
 * Rueckgabewerte:
 *     0, wenn dort keine Muenze platziert ist
 *     1, wenn dort eine Muenze platziert ist */
char muenze_platziert(struct simulation *s, int x, int y)
{
    int i;

    for (i = 0; i < s->anzahl_muenzen; i++)
        if (x == (int) s->muenzen[i].x && y == (int) s->muenzen[i].y)
            return (1);

    return (0);
}

/* Beschreibung:
 *     platziert die Robuttons auf dem Tisch
 * Eingabewerte:
 *     Zeiger auf die Simulationsstruktur
 * Rueckgabewerte:
 *     keine */
void platziere_robuttons(struct simulation *s)
{
    int i, x, y;
    double b;

    for (i = 0; i < s->anzahl_robuttons; i++) {
        do {
            x = (int) ((double) F_BREITE * drand48());
            y = (int) ((double) F_HOEHE * drand48());
        } while (!s->tischflaeche[y][x]
                 || robutton_platziert(s, x, y)
                 || muenze_platziert(s, x, y));

        s->robuttons[i].position.x = x;
        s->robuttons[i].position.y = y;

        b = 2.0 * M_PI * drand48();

        s->robuttons[i].richtung.x = cos(b);
        s->robuttons[i].richtung.y = sin(b);
    }
}

/* Beschreibung:
 *     platziert die Muenzen auf dem Tisch
 * Eingabewerte:
 *     Zeiger auf die Simulationsstruktur
 * Rueckgabewerte:
 *     keine */
void platziere_muenzen(struct simulation *s)
{
    int i;
    int x, y;

    for (i = 0; i < s->anzahl_muenzen; i++) {
        do {
            x = (int) (F_BREITE * drand48());
            y = (int) (F_HOEHE * drand48());
        } while (!s->tischflaeche[y][x]
                 || muenze_platziert(s, x, y)
                 || robutton_platziert(s, x, y));

        s->muenzen[i].x = x;
        s->muenzen[i].y = y;
    }
}

/* Beschreibung:
 *     aktualisiert die Ausgabe
 * Eingabewerte:
 *     Zeiger auf die Simulationsstruktur
 * Rueckgabewerte:
 *     keine */
void update_ausgabe(struct simulation *s)
{
    int i, j, k, l, x, y;
    char zoomlevel;

    zoomlevel = F_BREITE / (s->bis_x - s->von_x);

    for (i = 0, y = s->von_y; i <= F_HOEHE - zoomlevel; i += zoomlevel, y++)
        for (j = 0, x = s->von_x; j <= F_BREITE - zoomlevel;
             j += zoomlevel, x++)
            for (k = 0; k < zoomlevel; k++)
                for (l = 0; l < zoomlevel; l++)
                    if (x >= s->bis_x || y >= s->bis_y)
                        setze_pixel(s->screen, j + l,
                                    i + k,
                                    SDL_MapRGB(s->screen->format,
                                               BRAUN));
                    else {
                        if (!s->tischflaeche[y][x])
                            setze_pixel(s->screen,
                                        j + l,
                                        i + k,
                                        SDL_MapRGB(s->screen->format,
                                                   BRAUN));
                        else
                            setze_pixel(s->screen,
                                        j + l,
                                        i + k,
                                        SDL_MapRGB(s->screen->format,
                                                   GELB));
                    }

    for (i = 0; i < s->anzahl_muenzen; i++) {
        if ((int) s->muenzen[i].x < s->von_x
            || (int) s->muenzen[i].x >= s->bis_x
            || (int) s->muenzen[i].y < s->von_y
            || (int) s->muenzen[i].y >= s->bis_y)
            continue;

        for (k = 0; k < zoomlevel; k++)
            for (l = 0; l < zoomlevel; l++)
                setze_pixel(s->screen,
                            ((int) s->muenzen[i].x -
                             s->von_x) * zoomlevel + l,
                            ((int) s->muenzen[i].y -
                             s->von_y) * zoomlevel + k,
                            SDL_MapRGB(s->screen->format, BLAU));
    }

    for (i = 0; i < s->anzahl_robuttons; i++) {
        if ((int) s->robuttons[i].position.x < s->von_x
            || (int) s->robuttons[i].position.x >= s->bis_x
            || (int) s->robuttons[i].position.y < s->von_y
            || (int) s->robuttons[i].position.y >= s->bis_y)
            continue;

        if (s->robuttons[i].hat_muenze)
            for (k = 0; k < zoomlevel; k++)
                for (l = 0; l < zoomlevel; l++)
                    setze_pixel(s->screen,
                                ((int) s->robuttons[i].position.x -
                                 s->von_x) * zoomlevel + l,
                                ((int) s->robuttons[i].position.y -
                                 s->von_y) * zoomlevel + k,
                                SDL_MapRGB(s->screen->format, GRUEN));
        else
            for (k = 0; k < zoomlevel; k++)
                for (l = 0; l < zoomlevel; l++)
                    setze_pixel(s->screen,
                                ((int) s->robuttons[i].position.x -
                                 s->von_x) * zoomlevel + l,
                                ((int) s->robuttons[i].position.y -
                                 s->von_y) * zoomlevel + k,
                                SDL_MapRGB(s->screen->format, ROT));
    }

    SDL_Flip(s->screen);
}

/* Beschreibung:
 *     holt Speicher fuer eine neue Simulationsstruktur und initialisiert sie
 * Eingabewerte:
 *     1. Argument: der Tisch
 *     2. Argument: Anzahl der Robuttons
 *     3. Argument: Anzahl der Muenzen
 * Rueckgabewerte:
 *     Zeiger auf die erstellte Simulationsstruktur */
struct simulation *neue_simulation(struct tisch *t, int anzahl_robuttons,
                                   int anzahl_muenzen)
{
    struct simulation *s;
    char temp[1000] = {0};

    if ((s = calloc(1, sizeof(struct simulation))) == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    s->robuttons = neue_robuttons(anzahl_robuttons);
    s->neue_positionen = neue_vektoren(anzahl_robuttons);
    s->muenzen = neue_vektoren(anzahl_muenzen);
    s->tisch = t;
    s->frameskip = 0;
    s->von_x = 0;
    s->bis_x = F_BREITE;
    s->von_y = 0;
    s->bis_y = F_HOEHE;
    s->anzahl_robuttons = anzahl_robuttons;
    s->anzahl_muenzen = anzahl_muenzen;

    bestimme_tischflaeche(s);
    platziere_robuttons(s);
    platziere_muenzen(s);

    s->screen = SDL_SetVideoMode(F_BREITE, F_HOEHE, 0,
                                 SDL_HWSURFACE | SDL_DOUBLEBUF);

    if (s->screen == NULL) {
        fprintf(stderr, "Konnte Videomodus nicht setzen: %s\n",
                SDL_GetError());
        exit(EXIT_FAILURE);
    }
    
    sprintf(temp, "%d Robuttons %d Muenzen Tick: 0",
            s->anzahl_robuttons, s->anzahl_muenzen);

    SDL_WM_SetCaption(temp, temp);

    update_ausgabe(s);

    return (s);
}

/* Beschreibung:
 *     gibt den Speicher fuer eine Simulationsstruktur frei
 * Eingabewerte
 *     Zeiger auf die Simulationsstruktur
 * Rueckgabewerte:
 *     keine */
void simulation_freigeben(struct simulation *s)
{
    free(s->robuttons);
    free(s->neue_positionen);
    free(s->muenzen);
    tisch_freigeben(s->tisch);

    free(s);
}

/* Beschreibung:
 *     bewegt die Robuttons in der Richtung ihres Richtungsvektors
 * Eingabewerte:
 *     Zeiger auf die Simulationsstruktur
 * Rueckgabewerte:
 *     keine */
void bestimme_neue_positionen(struct simulation *s)
{
    int i;

    for (i = 0; i < s->anzahl_robuttons; i++) {
        s->neue_positionen[i].x =
            s->robuttons[i].position.x + s->robuttons[i].richtung.x;
        s->neue_positionen[i].y =
            s->robuttons[i].position.y + s->robuttons[i].richtung.y;
    }
}

/* Beschreibung:
 *     bewegt die Robuttons und behandelt moegliche Kollisionen
 * Eingabewerte:
 *     Zeiger auf die Simulationsstruktur
 * Ausgabewerte:
 *     keine */
void bewege_robuttons(struct simulation *s)
{
    char k1, k2, k3, r;
    int i, c;

    bestimme_neue_positionen(s);

    /* jeden Robutton pruefen */
    for (i = 0; i < s->anzahl_robuttons; i++) {
        r = 1;
        c = 0;

        do {
            /* Kollisionen behandeln */
            k1 = pruefe_kollision_muenzen(s, i);
            k2 = pruefe_kollision_robuttons(s, i);
            
            if (s->tisch->typ != KREIS)
                k3 = pruefe_kollision_tischkante_eckig(s, i);
            else
                k3 = pruefe_kollision_tischkante_rund(s, i);
            
            /* Endlosschleife vermeiden */
            c++;
            
            /* solange mindestens eine Kollision vorliegt */
        } while ((k1 || k2 || k3) && c < 100);
        
        if(c == 100) {
            /* Robutton ist "verkeilt" und bleibt stehen */
            s->neue_positionen[i].x = s->robuttons[i].position.x;
            s->neue_positionen[i].y = s->robuttons[i].position.y;
        }
    }

    /* die Robuttons werden bewegt */
    for (i = 0; i < s->anzahl_robuttons; i++) {
        s->robuttons[i].position.x = s->neue_positionen[i].x;
        s->robuttons[i].position.y = s->neue_positionen[i].y;
    }
}

/* Beschreibung:
 *     aendert die Variablen in der Simulationsstruktur,
 *     sodass das Sichtfeld nach links bewegt wird
 * Eingabewerte:
 *     Zeiger auf die Simulationsstruktur
 * Rueckgabewerte:
 *     keine */
void sichtfeld_nach_links(struct simulation *s)
{
    s->von_x -= 5;
    s->bis_x -= 5;

    if (s->von_x < 0) {
        s->von_x += 5;
        s->bis_x += 5;
    }
}

/* Beschreibung:
 *     aendert die Variablen in der Simulationsstruktur,
 *     sodass das Sichtfeld nach rechts bewegt wird
 * Eingabewerte:
 *     Zeiger auf die Simulationsstruktur
 * Rueckgabewerte:
 *     keine */
void sichtfeld_nach_rechts(struct simulation *s)
{
    s->von_x += 5;
    s->bis_x += 5;

    if (s->bis_x > F_BREITE) {
        s->von_x -= 5;
        s->bis_x -= 5;
    }
}

/* Beschreibung:
 *     aendert die Variablen in der Simulationsstruktur,
 *     sodass das Sichtfeld nach oben bewegt wird
 * Eingabewerte:
 *     Zeiger auf die Simulationsstruktur
 * Rueckgabewerte:
 *     keine */
void sichtfeld_nach_oben(struct simulation *s)
{
    s->von_y -= 5;
    s->bis_y -= 5;

    if (s->von_y < 0) {
        s->von_y += 5;
        s->bis_y += 5;
    }
}

/* Beschreibung:
 *     aendert die Variablen in der Simulationsstruktur,
 *     sodass das Sichtfeld nach unten bewegt wird
 * Eingabewerte:
 *     Zeiger auf die Simulationsstruktur
 * Rueckgabewerte:
 *     keine */
void sichtfeld_nach_unten(struct simulation *s)
{
    s->von_y += 5;
    s->bis_y += 5;

    if (s->bis_y > F_HOEHE) {
        s->von_y -= 5;
        s->bis_y -= 5;
    }
}

/* Beschreibung:
 *     aendert die Variablen in der Simulationsstruktur,
 *     sodass hineingezoomt wird
 * Eingabewerte:
 *     Zeiger auf die Simulationsstruktur
 * Rueckgabewerte:
 *     keine */

void hineinzoomen(struct simulation *s)
{
    char zoomlevel;

    zoomlevel = F_BREITE / (s->bis_x - s->von_x);

    if (zoomlevel >= MAX_ZOOMLEVEL)
        return;

    s->bis_x -= (s->bis_x - s->von_x) / 2;
    s->bis_y -= (s->bis_y - s->von_y) / 2;
}

/* Beschreibung:
 *     aendert die Variablen in der Simulationsstruktur,
 *     sodass hinausgezoomt wird
 * Eingabewerte:
 *     Zeiger auf die Simulationsstruktur
 * Rueckgabewerte:
 *     keine */
void hinauszoomen(struct simulation *s)
{
    char zoomlevel;

    zoomlevel = F_BREITE / (s->bis_x - s->von_x);

    if (zoomlevel == 1)
        return;

    s->bis_x += (s->bis_x - s->von_x);
    s->bis_y += (s->bis_y - s->von_y);

    if (s->bis_x > F_BREITE) {
        s->von_x -= s->bis_x - F_BREITE;
        s->bis_x = F_BREITE;
    }

    if (s->bis_y > F_HOEHE) {
        s->von_y -= s->bis_y - F_HOEHE;
        s->bis_y = F_HOEHE;
    }
}

/* Beschreibung:
 *     fragt die Tastendruecke ab und verarbeitet diese
 * Eingabewerte:
 *     Zeiger auf die Simulationsstruktur
 * Rueckgabewerte:
 *     keine */
void tasten_abfragen(struct simulation *s)
{
    Uint8 *keys;

    keys = SDL_GetKeyState(NULL);

    /* Scrollen */
    if (keys[SDLK_LEFT] == SDL_PRESSED) {
        sichtfeld_nach_links(s);
        update_ausgabe(s);
    }

    if (keys[SDLK_RIGHT] == SDL_PRESSED) {
        sichtfeld_nach_rechts(s);
        update_ausgabe(s);
    }

    if (keys[SDLK_UP] == SDL_PRESSED) {
        sichtfeld_nach_oben(s);
        update_ausgabe(s);
    }

    if (keys[SDLK_DOWN] == SDL_PRESSED) {
        sichtfeld_nach_unten(s);
        update_ausgabe(s);
    }

    /* Zoomen */
    if (keys[SDLK_KP_PLUS] == SDL_PRESSED
        || keys[SDLK_PLUS] == SDL_PRESSED) {
        hineinzoomen(s);
        update_ausgabe(s);
        SDL_Delay(300);
    }

    if (keys[SDLK_KP_MINUS] == SDL_PRESSED
        || keys[SDLK_MINUS] == SDL_PRESSED) {
        hinauszoomen(s);
        update_ausgabe(s);
        SDL_Delay(300);
    }

    /* Frameskip */
    if (keys[SDLK_r] == SDL_PRESSED)
        s->frameskip = 0; /* Frameskip zuruecksetzen */

    if (keys[SDLK_1] == SDL_PRESSED
        || keys[SDLK_KP1] == SDL_PRESSED)
        s->frameskip = MAX_FRAMESKIP / 100 * 10;

    if (keys[SDLK_2] == SDL_PRESSED
        || keys[SDLK_KP2] == SDL_PRESSED)
        s->frameskip = MAX_FRAMESKIP / 100 * 20;

    if (keys[SDLK_3] == SDL_PRESSED
        || keys[SDLK_KP3] == SDL_PRESSED)
        s->frameskip = MAX_FRAMESKIP / 100 * 30;

    if (keys[SDLK_4] == SDL_PRESSED
        || keys[SDLK_KP4] == SDL_PRESSED)
        s->frameskip = MAX_FRAMESKIP / 100 * 40;

    if (keys[SDLK_5] == SDL_PRESSED
        || keys[SDLK_KP5] == SDL_PRESSED)
        s->frameskip = MAX_FRAMESKIP / 100 * 50;

    if (keys[SDLK_6] == SDL_PRESSED
        || keys[SDLK_KP6] == SDL_PRESSED)
        s->frameskip = MAX_FRAMESKIP / 100 * 60;

    if (keys[SDLK_7] == SDL_PRESSED
        || keys[SDLK_KP7] == SDL_PRESSED)
        s->frameskip = MAX_FRAMESKIP / 100 * 70;

    if (keys[SDLK_8] == SDL_PRESSED
        || keys[SDLK_KP8] == SDL_PRESSED)
        s->frameskip = MAX_FRAMESKIP / 100 * 80;

    if (keys[SDLK_9] == SDL_PRESSED
        || keys[SDLK_KP9] == SDL_PRESSED)
        s->frameskip = MAX_FRAMESKIP / 100 * 90;

    if (keys[SDLK_0] == SDL_PRESSED
        || keys[SDLK_KP0] == SDL_PRESSED)
        s->frameskip = MAX_FRAMESKIP;

    if (keys[SDLK_RCTRL] == SDL_PRESSED)
        if (s->frameskip < MAX_FRAMESKIP)
            s->frameskip += 1;

    if (keys[SDLK_LCTRL] == SDL_PRESSED)
        if (s->frameskip > 0)
            s->frameskip -= 1;
}

/* Beschreibung:
 *     liest die Konfigurationsdatei ein und erstellt
 *     eine neue Simulationsstruktur
 * Eingabewerte:
 *     Dateiname der Konfigurationsdatei
 * Rueckgabewerte:
 *     Zeiger auf die erstellte Simulationsstruktur */
struct simulation *neue_simulation_von_datei(char *dateiname)
{
    struct simulation *s;
    struct vektor *ecken;
    double r;
    char typ;
    int i, j, anzahl_r, anzahl_m, anzahl_e;
    char zeile[LINE_MAX];
    struct tisch *t;
    FILE *datei;

    /* Datei oeffnen */
    if ((datei = fopen(dateiname, "r")) == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    t = NULL;
    ecken = NULL;
    anzahl_r = anzahl_m = anzahl_e = i = j = 0;
    typ = -1;

    while (fgets(zeile, LINE_MAX, datei) != NULL) {
        if (zeile[0] == '#')
            continue;           /* Kommentar */

        switch (i) {
        case 0:
            /* Typ einlesen */
            if (strncmp(zeile, "Kreis", 5) == 0)
                typ = KREIS;
            if (strncmp(zeile, "Viereck", 7) == 0)
                typ = VIERECK;
            if (strncmp(zeile, "Polygon", 7) == 0)
                typ = POLYGON;

            if (typ == -1) {
                fprintf(stderr, "fehlerhafte Konfigurationsdatei\n");
                exit(EXIT_FAILURE);
            }
            break;

        case 1:
            /* Anzahl der Robuttons einlesen */
            anzahl_r = atoi(zeile);

            if (anzahl_r <= 0) {
                fprintf(stderr, "falsche Anzahl an Robuttons\n");
                exit(EXIT_FAILURE);
            }
            break;

        case 2:
            /* Anzahl der Muenzen einlesen */
            anzahl_m = atoi(zeile);

            if (anzahl_m <= 0) {
                fprintf(stderr, "falsche Anzahl an Muenzen\n");
                exit(EXIT_FAILURE);
            }
            break;

        case 3:
            switch (typ) {
            case KREIS:
                /* Mittelpunkt einlesen */
                ecken = neue_vektoren(1);
                sscanf(zeile, "%lf %lf", &(ecken[j].x), &(ecken[j].y));
                j++;
                break;

            case VIERECK:
                /* Ecke einlesen */
                ecken = neue_vektoren(2);
                sscanf(zeile, "%lf %lf", &(ecken[j].x), &(ecken[j].y));
                j++;
                break;

            case POLYGON:
                /* Anzahl der Ecken einlesen */
                anzahl_e = atoi(zeile) + 1;

                if (anzahl_e <= 1) {
                    fprintf(stderr, "falsche Anzahl an Ecken\n");
                    exit(EXIT_FAILURE);
                }

                ecken = neue_vektoren(anzahl_e);
                break;
            }

            break;

        default:
            if (typ == KREIS)
                /* Radius einlesen */
                sscanf(zeile, "%lf", &r);
            else
                /* Ecke einlesen */
                sscanf(zeile, "%lf %lf", &(ecken[j].x), &(ecken[j].y));

            j++;
        }

        i++;
    }
    
    /* Tisch erstellen */
    switch (typ) {
    case KREIS:
        t = neuer_runder_tisch(ecken[0], r);
        break;

    case VIERECK:
        t = neuer_viereckiger_tisch(ecken[0], ecken[1]);
        break;

    case POLYGON:
        ecken[anzahl_e - 1].x = ecken[0].x;
        ecken[anzahl_e - 1].y = ecken[0].y;
        t = neuer_poly_tisch(ecken, anzahl_e);
        break;
    }

    /* Simulation erstellen */
    s = neue_simulation(t, anzahl_r, anzahl_m);

    /* Speicher freigeben */
    if (typ == KREIS || typ == VIERECK)
        free(ecken);

    /* Datei schliessen */
    fclose(datei);

    /* erstellte Simulation zurueckgeben */
    return (s);
}

/* Beschreibung:
 *     - erstellt ein Fenster und startet/verwaltet die Simulation
 *     - platziert die Robuttons und Muenzen
 *     - bewegt die Robuttons und fragt eventuelle Tastendruecke ab
 *     - die Funktion/Simulation bleibt solange aktiv
 *       bis das Fenster geschlossen wird
 * Eingabewerte:
 *     der Dateiname der Konfigurationsdatei fuer die Simulation
 * Rueckgabewerte:
 *     keine */
void simulation_starten(char *dateiname)
{
    SDL_Event event;
    char beenden;
    int i;
    long tick;
    struct simulation *s;
    char title[1000];

    /* SDL starten */
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_NOPARACHUTE)) {
        perror("SDL_Init");
        exit(EXIT_FAILURE);
    }

    /* Initialisierungen */
    srand48(time(NULL));
    beenden = 0;
    tick = 0L;
    
    /* neue Simulationsstruktur erstellen */
    s = neue_simulation_von_datei(dateiname);

    /* solange Fenster nicht geschlossen wird */
    while (!beenden) {
        /* Ausgabe aktualisieren */
        update_ausgabe(s);

        for (i = 0; i <= s->frameskip && !beenden; i++) {
            /* naechster Schritt der Simulation */
            bewege_robuttons(s);
            tick++;

            /* besseres Scrollen ermoeglichen */
            if (s->frameskip == 0)
                tasten_abfragen(s);

            /* Events abfragen */
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    beenden = 1; /* Fenster wurde geschlossen */
                    break;
                }

                /* Taste wurde gedrueckt */
                if (s->frameskip != 0 && event.type == SDL_KEYDOWN)
                    tasten_abfragen(s);
            }
        }
        
        sprintf(title, "%d Robuttons %d Muenzen Tick: %ld",
                s->anzahl_robuttons, s->anzahl_muenzen, tick);
        
        SDL_WM_SetCaption(title, NULL);
    }

    /* Speicher freigeben */
    simulation_freigeben(s);

    /* SDL beenden */
    SDL_Quit();
}
