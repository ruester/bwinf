#include <stdio.h>              /* fuer printf/fscanf/perror/... */
#include <stdlib.h>             /* fuer malloc/free/exit/... */
#include <string.h>             /* fuer strlen/strtok/strcat/... */
#include <unistd.h>             /* fuer getopt/optopt/optarg */
#include <ctype.h>              /* fuer toupper/tolower */

/*
 * Makros:
 */
#define BOOL unsigned char
#define TRUE (1)
#define FALSE (0)
#define DICT "english_pspad.dict"
#define BUCH "holy_bible.txt"

/*
 * Funktionen:
 */
void print_usage();
struct List *new_list();
struct Element *new_element(char *s);
void add_element(struct List *l, char *s);
void print_list(struct List *l);
void delete_list(struct List *l);
void print_usage();
struct List *finde_moegliche_woerter(char *dict, char *s);
BOOL hat_vokale(char *str);
char *differenzmenge(char *a, char *b);
void finde_anagramme(char *eingabe, struct List *moeglich,
                     struct List *anagramme, char *add,
                     int level, int max_depth);
int vergleiche(char *a, char *b);
int suche_wort(char *str, struct List *l);
int **zaehle(struct List *l, char **dateien, int anzahl);
struct List *filter(struct List *m, struct List *ana,
                    char **dateien, int anzahl, BOOL streng);
char **teilen(char *str, int *anzahl);

/* Struktur Element:
 *  - stellt ein Element einer Liste dar
 *  - enthaelt einen String
 *  - enthaelt einen Zeiger auf das naechste Element
 */
struct Element {
    char *str;                  /* String */
    struct Element *next;       /* Zeiger auf Nachfolger */
};

/* Struktur List:
 *  - stellt eine Liste dar
 *  - enthaelt einen Zeiger auf das erste und Letzte Element der Liste
 *  - enthaelt eine Variable fuer die Anzahl der Element
 */
struct List {
    long count;                 /* Anzahl der Elemente in der Liste */
    struct Element *first;      /* Zeiger auf erstes Element */
    struct Element *last;       /* Zeiger auf letztes Element */
};

/*
 * Funktion alloziert Speicher fuer eine Liste und
 * gibt den Zeiger auf die Liste zurueck.
 */
struct List *new_list()
{
    struct List *new;           /* Zeiger auf eine Liste */

    /* Speicher allozieren */
    if ((new = malloc(sizeof(struct List))) == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    /* initialisieren der Liste */
    new->first = NULL;
    new->last = NULL;
    new->count = 0;

    /* Zeiger der Liste zurueckgeben */
    return (new);
}

/*
 * Funktion alloziert Speicher fuer ein Element und
 * gibt den Zeiger auf das Element zurueck.
 */
struct Element *new_element(char *s)
{
    struct Element *new;        /* Zeiger auf ein Element */

    /* Speicher allozieren */
    if ((new = malloc(sizeof(struct Element))) == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    /* initialiseren des Elements */
    new->next = NULL;
    new->str = s;

    /* Zeiger des Elements zurueckgeben */
    return (new);
}

/*
 * Funktion fuegt einen String (in Form eines Elements) an eine Liste.
 * Das Element wird dabei hinten an die Liste angefuegt.
 */
void add_element(struct List *l, char *s)
{
    if (l->count == 0) {
        /* kein Element in Liste */
        l->first = new_element(s);
        l->last = l->first;
    } else {
        /* Element hinten an Liste anfuegen */
        l->last->next = new_element(s);
        l->last = l->last->next;
    }

    /* Zaehlvariable erhoehen */
    l->count++;
}

/*
 * Funktion gibt die Strings der Elemente einer Liste aus.
 */
void print_list(struct List *l)
{
    struct Element *current;    /* Zeiger auf ein Element */

    if (l == NULL || l->count == 0)
        return;                 /* Liste ist leer */

    /* Zeiger zeigt auf erstes Element der Liste */
    current = l->first;

    /* Liste durchgehen */
    while (current != NULL) {
        /* Ausgeben des Strings */
        printf("%s\n", current->str);

        /* naechstes Element */
        current = current->next;
    }

    /* Anzahl der Elemente ausgeben */
    printf("Anzahl: %ld\n", l->count);
}

/*
 * Funktion loescht eine Liste.
 */
void delete_list(struct List *l)
{
    struct Element *current,    /* Zeiger auf ein Element */
    *old;                       /* Zeiger auf ein zu loeschendes Element */

    if (l == NULL)
        return;                 /* Liste ist leer */

    /* Zeiger zeigt auf erstes Element der Liste */
    current = l->first;

    /* Liste durchgehen */
    while (current != NULL) {
        /* zu loeschendes Element ist das aktuelle Element */
        old = current;

        /* naechstes Element */
        current = current->next;

        /* Speicher des "alten" Elements freigeben */
        free(old->str);
        free(old);
    }

    /* Speicher der Liste freigeben */
    free(l);
    l = NULL;
}

/*
 * Funktion gibt eine Hilfe zur Benutzung des Programms aus.
 */
void print_usage()
{
    printf("Benutzung: ./anagramme -w \"<Wort(gruppe)>\" [Optionen]\n");
    printf("Optionen:\n");
    printf("-a <Anzahl>\t\tmaximale Anzahl der Woerter pro Anagramm\n");
    printf("\t\t\tDefault: 0 (kein Limit)\n\n");
    printf("-d <Dateiname>\t\tDateiname des Woerterbuches\n");
    printf("\t\t\tDefault: %s\n\n", DICT);
    printf("-b \"<Dateien>\"\t\tTextdateien, die beim Herausfinden\n");
    printf("\t\t\tsinnvoller Anagramme benutzt werden sollen\n");
    printf("\t\t\tDefault: %s\n\n", BUCH);
    printf("-p\t\t\tAusgeben von Zwischenergebnissen\n\n");
    printf("-s\t\t\tStrenges Aussortieren");
    printf(" von nicht sinnvollen Anagrammen\n\n");
    printf("-h\t\t\tAusgeben dieser Hilfe\n\n");
}

/*
 * Funktion findet Woerter, aus denen ein Anagramm gebildet werden kann und
 * traegt diese in eine Liste ein, welche zurueckgegeben wird.
 * Dabei wird ein Woerterbuch verwendet,
 * dessen Dateiname im ersten Argument angegeben wird.
 */
struct List *finde_moegliche_woerter(char *dict, char *eingabe)
{
    FILE *wb;                   /* Zeiger auf eine Datei */
    char aktuell[81];           /* zum Auslesen der Zeilen der Datei */
    char *rest;                 /* Differenzmenge */
    char *kopie;                /* Kopie von aktuell */
    struct List *l;             /* Zeiger auf eine Liste */

    /* neue Liste erzeugen */
    l = new_list();

    /* Woerterbuch oeffnen */
    if ((wb = fopen(dict, "r")) == NULL) {
        printf("\rWoerterbuch konnte nicht geoeffnet werden!\n");
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    /* Zeilen des Woerterbuches durchgehen */
    while (fscanf(wb, "%80s", aktuell) != EOF) {
        /* Differenzmenge bilden */
        rest = differenzmenge(eingabe, aktuell);

        /* Differenzmenge konnte nicht errechnet werden */
        if (rest == NULL)
            continue;

        /* zur Liste hinzufuegen,
           wenn das Wort Vokale hat */
        if (hat_vokale(aktuell)) {
            /* kopie von aktuell anlegen */
            if ((kopie = strdup(aktuell)) == NULL) {
                perror("strdup");
                exit(EXIT_FAILURE);
            }

            /* aktuelles Wort der Liste hinzufuegen */
            add_element(l, kopie);
        }

        /* Speicher freigeben */
        free(rest);
    }

    /* Datei schliessen */
    fclose(wb);

    /* Zeiger auf Liste zurueckgeben */
    return (l);
}

/*
 * Funktion prueft den uebergebenen String auf Vokale.
 * Funktion gibt TRUE zurueck, wenn im String mindestens ein Vokal vorkommt,
 * andernfalls wird FALSE zurueckgegeben.
 */
BOOL hat_vokale(char *str)
{
    char *i;                    /* Zeiger auf einen Buchstaben */

    /* alle Buchstaben des Strings durchlaufen */
    for (i = str; *i; i++) {
        /* pruefen, ob aktueller Buchstaben ein Vokal ist */
        if (tolower(*i) == 'a' || tolower(*i) == 'e' || tolower(*i) == 'i'
            || tolower(*i) == 'o' || tolower(*i) == 'u')
            return (TRUE);      /* 'wahr' zurueckgeben */
    }

    /* 'falsch' zurueckgeben */
    return (FALSE);
}

/*
 * Funktion gibt die Differenzmenge zweier Strings zurueck.
 * Es wird NULL zurueckgegeben,
 * wenn die Differenzmenge nicht errechnet werden konnte,
 * ansonsten wird der String a ohne b zurueckgegeben.
 * Wenn beide Strings gleich sind, dann wird "\0" zurueckgegeben.
 */
char *differenzmenge(char *a, char *b)
{
    char *a_kopie,              /* Kopie vom 1. Argument */
    *b_kopie,                   /* Kopie vom 2. Argument */
    *i, *j,                     /* Pointer zum Durchlaufen der Strings */
    *r;                         /* Rueckgabewert */
    int count, k;               /* Zaehlvariablen */
    BOOL gestrichen;            /* zum fruehen Abbrechen notwendig */

    /* 2. Argument ist die leere Menge */
    if (b == NULL) {
        /* Kopie von a anlegen */
        if ((a_kopie = strdup(a)) == NULL) {
            perror("strdup");
            exit(EXIT_FAILURE);
        }

        return (a_kopie);       /* 1. Argument zurueckgeben */
    }

    /* Kopien von den Argumenten anlegen */
    a_kopie = strdup(a);
    b_kopie = strdup(b);

    if (a_kopie == NULL || b_kopie == NULL) {
        perror("strdup");
        exit(EXIT_FAILURE);
    }

    /* b durchlaufen */
    for (i = b_kopie; *i; i++) {
        /* Leerzeichen ueberspringen */
        if (*i == ' ')
            continue;

        /* noch keine Buchstaben gestrichen */
        gestrichen = FALSE;

        /* a durchlaufen */
        for (j = a_kopie; *j; j++) {
            /* Leerzeichen ueberspringen */
            if (*j == ' ')
                continue;

            /* Buchstaben vergleichen */
            if (tolower(*i) == tolower(*j)) {
                /* Buchstaben "streichen" */
                *i = ' ';
                *j = ' ';

                /* es wurden Buchstaben gestrichen */
                gestrichen = TRUE;

                /* naechster Buchstabe von b */
                break;
            }
        }

        /* prufen, ob Buchstabe von b in a enthalten war */
        if (gestrichen == FALSE) {
            /* Speicher freigeben */
            free(a_kopie);
            free(b_kopie);

            /* b ist keine Teilmenge von a */
            return (NULL);
        }
    }

    /* uebrig gebliebene Buchstaben in a zaehlen */
    count = 0;
    for (i = a_kopie; *i; i++)
        if (*i != ' ')
            count++;

    /* Speicher holen */
    if ((r = malloc((count + 1) * sizeof(char))) == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    /* String nullterminieren */
    r[count] = '\0';

    /* uebrige Buchstaben von a in r kopieren */
    for (i = a_kopie, k = 0; *i; i++)
        if (*i > 0 && *i != ' ')
            r[k++] = *i;

    /* Speicher freigeben */
    free(a_kopie);
    free(b_kopie);

    /* a ohne b zurueckgeben */
    return (r);
}

/*
 * Funktion findet Anagramme anhand einer Eingabe und einer Liste von
 * moeglichen Woertern, aus denen das Anagramm bestehen koennte.
 * Die Funktionarbeitet rekursiv, weshalb man eine maximale Tiefe angeben kann.
 * Die maximale Tiefe steht in dem Fall auch fuer die maximale
 * Anzahl der Woerter in den Anagrammen.
 * Die Anagramme werden in einer Liste gespeichert, deren Zeiger als
 * 3. Argument uebergeben wird.
 * Das 4. und 5. Argument muss beim ersten Aufruf NULL bzw. 0 sein.
 */
void finde_anagramme(char *eingabe, struct List *moeglich,
                     struct List *anagramme, char *add,
                     int level, int max_depth)
{
    char *rest,                 /* Differenzmenge */
    *kopie,                     /* Kopie von add */
    *temp;                      /* Hilfsvariable */
    struct Element *current;    /* Zeiger auf aktuelles Listenelement */

    /* Zeiger auf erstes Element zeigen lassen */
    current = moeglich->first;

    /* maximale Anzahl an Woertern pro Anagramm nicht ueberschreiten */
    if (max_depth > 0 && level >= max_depth) {
        /* Speicher freigeben */
        free(add);

        /* Funktion beenden */
        return;
    }

    /* die Liste durchgehen */
    while (current != NULL) {
        /* current->str an kopie haengen */
        if (add == NULL) {
            if ((kopie = strdup(current->str)) == NULL) {
                perror("strdup");
                exit(EXIT_FAILURE);
            }
        } else {
            /* Kopie von add erstellen */
            if ((kopie = strdup(add)) == NULL) {
                perror("strdup");
                exit(EXIT_FAILURE);
            }

            /* Speicher vergroessern */
            if ((kopie = realloc(kopie, (strlen(kopie) +
                                         strlen(current->str) +
                                         2) * sizeof(char))) == NULL) {
                perror("realloc");
                exit(EXIT_FAILURE);
            }

            /* Leerzeichen anhaengen */
            strcat(kopie, " ");

            /* current->str anhaengen */
            strcat(kopie, current->str);
        }

        /* Differenzmenge bestimmen */
        rest = differenzmenge(eingabe, kopie);

        /* kopie ist keine Teilmenge von eingabe */
        if (rest == NULL) {
            /* naechstes Listenelement */
            current = current->next;

            /* Speicher freigeben */
            free(kopie);

            /* naechstes Wort probieren */
            continue;
        }

        /* Anagramm gefunden */
        if (*rest == '\0') {
            printf("\r%s               \r", kopie);
            fflush(stdout);

            /* Kopie anlegen */
            if ((temp = strdup(kopie)) == NULL) {
                perror("strdup");
                exit(EXIT_FAILURE);
            }

            /* Anagramm */
            add_element(anagramme, temp);

            /* naechstes Listenelement */
            current = current->next;

            /* Speicher freigeben */
            free(kopie);
            free(rest);

            /* naechstes Wort probieren */
            continue;
        }

        /* rest auf Vokale pruefen */
        if (hat_vokale(rest) == TRUE) {
            /* Kopie anlegen */
            if ((temp = strdup(kopie)) == NULL) {
                perror("strdup");
                exit(EXIT_FAILURE);
            }

            /* rekursiver Aufruf */
            finde_anagramme(eingabe, moeglich,
                            anagramme, temp, level + 1, max_depth);
        }

        /* Speicher freigeben */
        free(rest);
        free(kopie);

        /* naechstes Listenelement */
        current = current->next;
    }

    /* Speicher freigeben */
    free(add);
}

/*
 * Funktion vergleicht zwei Strings.
 * Es wird 0 zurueckgegeben, wenn die Strings genau gleich sind.
 * Beim Vergleichen wird nicht auf Gross- und Kleinschreibung geachtet.
 * Es wird 1 zurueckgegeben, wenn die Strings sich mindestens
 * in einem Buchstaben unterscheiden.
 */
int vergleiche(char *a, char *b)
{
    char *i, *j;                /* Zeiger zum Durchlaufen der Strings */

    /* vorzeitig abbrechen */
    if (a == NULL || b == NULL || strlen(a) != strlen(b))
        return (1);             /* Strings sind nicht gleich */

    /* Strings vergleichen */
    for (i = a, j = b; *i && *j; i++, j++)
        /* Gross- und Kleinschreibung ignorieren */
        if (tolower(*i) != tolower(*j))
            return (1);         /* Strings sind nicht gleich */

    /* Strings sind gleich */
    return (0);
}

/*
 * Funktion sucht ein Wort in einer Liste und
 * gibt den Index des Elements zurueck.
 * Beim Suchen des Wortes wird nicht auf Gross- und Kleinschreibung geachtet.
 */
int suche_wort(char *str, struct List *l)
{
    int i;                      /* Zaehlvariable */
    struct Element *current;    /* Zeiger auf Listenelement */

    /* Zeiger auf erstes Element der Liste zeigen lassen */
    current = l->first;

    /* String suchen und mitzaehlen */
    i = 0;
    while (current != NULL) {
        /* String des Elements mit dem Gesuchten vergleichen */
        if (vergleiche(str, current->str) == 0)
            return (i);         /* Index zurueckgeben */

        /* naechstes Listenelement */
        current = current->next;

        /* Index erhoehen */
        i++;
    }

    /* Wort nicht in Liste gefunden */
    return (-1);
}

/*
 * Funktion zaehlt wie oft ein Wort Nachfolger eines anderen Wortes ist.
 * Die Dateien, die durchsucht werden sollen, werden als Parameter angegeben.
 * Es werden nur die Woerter betrachtet, die in der Liste sind.
 * Es wird ein 2-dimensionales Array zurueckgegeben,
 * wo alle noetigen Informationen gespeichert sind.
 */
unsigned int **zaehlen(struct List *l, char **dateien, int anzahl)
{
    unsigned int **a;           /* Zeiger auf 2-dimensionales Array */
    char wort[80];              /* zum Auslesen der Dateien */
    int i, j,                   /* Zaehlvariablen */
     index1, index2;            /* Indizes */
    FILE *z;                    /* Zeiger auf Datei */

    /* Speicher holen fuer Zeilen */
    if ((a = malloc(l->count * sizeof(int *))) == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    /* Speicher holen fuer Spalten */
    for (i = 0; i < l->count; i++) {
        if ((a[i] = malloc(l->count * sizeof(int))) == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
    }

    /* Array initialisieren */
    for (i = 0; i < l->count; i++)
        for (j = 0; j < l->count; j++)
            a[i][j] = 0;

    /* alle Dateien durchgehen */
    for (i = 0; i < anzahl; i++) {
        /* Datei oeffnen */
        if ((z = fopen(dateien[i], "r")) == NULL) {
            perror("\rfopen");
            continue;
        }

        /* erstes Wort auslesen */
        if (fscanf(z, "%80s", wort) == EOF) {
            printf("\rUnerwartetes Dateiende!\n");
            exit(EXIT_FAILURE);
        }

        /* Wort in Liste suchen */
        index1 = suche_wort(wort, l);

        /* naechstes Wort auslesen */
        while (fscanf(z, "%80s", wort) != EOF) {
            /* Wort in Liste suchen */
            index2 = suche_wort(wort, l);

            /* wenn beide Woerter in Liste vorhanden sind */
            if (index1 >= 0 && index2 >= 0)
                /* es wird gezaehlt, dass wort2 auf wort1 folgte */
                a[index1][index2]++;

            /* erstes Wort ist nun das zweite Wort */
            index1 = index2;
        }

        /* Datei schliessen */
        fclose(z);
    }

    /* Array zurueckgeben */
    return (a);
}

/*
 * Funktion sortiert nicht sinnvolle Anagramme aus einer Liste aus,
 * dabei wird die Funktion zaehlen benutzt.
 * Alle Anagramme, die mindestens eine Wortgruppe (2 Woerter) besitzen, die
 * in einem Text vorkam werden dabei als sinnvoll betrachtet.
 * Soll strenger aussortiert werden, wird das letzte Argument auf TRUE gesetzt.
 * Dann muessen _alle_ Wortgruppen eines Anagramms in Texten vorgekommen sein.
 * Die zu durchsuchenden Texte werden durch das 3. Argument angegeben.
 * Es wird eine Liste zurueckgegeben, die alle sinvollen Anagramme enthaelt.
 */
struct List *filter(struct List *m, struct List *ana,
                    char **dateien, int anzahl, BOOL streng)
{
    unsigned int **a;           /* 2-dimensionales Array */
    int i,                      /* Zaehlvariable */
     wert,                      /* Hilfsvariable */
     index1, index2;            /* Indizes */
    struct List *f;             /* Liste die zurueckgegeben wird */
    char *token,                /* Zeiger zum herausfinden der Woerter des Anagramms */
    *kopie,                     /* Kopie eines Anagramms */
    *temp;                      /* Hilfsvariable */
    char delims[] = " ";        /* fuer strtok */
    struct Element *current;    /* Zeiger auf ein Listenelement */

    /* neue Liste erstellen */
    f = new_list();

    /* keine Anagramme in Liste */
    if (ana->count == 0)
        return (f);             /* Liste zurueckgeben */

    /* Wortgruppen aus Texten zaehlen */
    a = zaehlen(m, dateien, anzahl);

    /* Zeiger auf erstes Listenelement zeigen lassen */
    current = ana->first;

    /* Liste der Anagramme durchgehen */
    while (current != NULL) {
        /* auf strenge Aussortierung pruefen */
        if (streng == TRUE)
            wert = 1;
        else
            wert = 0;

        /* Kopie vom Anagramm anlegen */
        if ((kopie = strdup(current->str)) == NULL) {
            perror("strdup");
            exit(EXIT_FAILURE);
        }

        /* Anagramm-Woerter herausfinden */
        token = strtok(kopie, delims);

        /* token1 in Liste suchen und Index merken */
        index1 = suche_wort(token, m);

        /* Anagramm besteht nur aus einem Wort */
        if (strlen(token) == strlen(current->str)) {
            wert = 1;           /* Anagramm wird in Liste aufgenommen */
            token = NULL;       /* Schleife ueberspringen */
        }

        /* alle Woerter des Anagramms durchgehen */
        while (token != NULL) {
            /* naechstes Wort im Anagramm herausfinden */
            if ((token = strtok(NULL, delims)) == NULL)
                break;

            /* Index des Wortes im Array herausfinden */
            index2 = suche_wort(token, m);

            /* Wert-Berechnung differenzieren */
            if (streng == TRUE)
                wert *= a[index1][index2];
            else
                wert += a[index1][index2];

            /* das erste Wort ist nun das Zweite */
            index1 = index2;
        }

        /* pruefen, ob Anagramm sinvoll ist */
        if (wert >= 1) {
            /* Kopie von current->str anlegen */
            if ((temp = strdup(current->str)) == NULL) {
                perror("strdup");
                exit(EXIT_FAILURE);
            }

            /* Anagramm wird in die Liste uebernommen */
            add_element(f, temp);
        }

        /* Speicher freigeben */
        free(kopie);

        /* naechstes Listenelement */
        current = current->next;
    }

    /* Speicher freigeben */
    for (i = 0; i < m->count; i++)
        free(a[i]);
    free(a);

    /* Liste sinnvoller Anagramme zurueckgeben */
    return (f);
}

/*
 * Funktion teilt einen String an den Stellen, wo Leerzeichen sind.
 * Die Funktion macht daraus ein Array von Strings und gibt dieses zurueck.
 * Im 2. Argument wird die Anzahl der Strings gespeichert.
 */
char **teilen(char *str, int *anzahl)
{
    char **r;                   /* Array, das zurueckgegeben wird */
    char *token,                /* Zeiger auf ein Token */
    *kopie;                     /* Kopie des 1. Arguments */
    char delims[] = " ";        /* fuer strtok */

    /* initialisieren */
    *anzahl = 0;
    r = NULL;
    if ((kopie = strdup(str)) == NULL) {
        perror("strdup");
        exit(EXIT_FAILURE);
    }

    /* ersten String holen */
    if ((token = strtok(kopie, delims)) == NULL)
        return (NULL);

    /* alle Strings durchgehen */
    while (token != NULL) {
        /* ein String wurde gefunden */
        (*anzahl)++;

        /* Speicher holen */
        if ((r = realloc(r, sizeof(char *) * (*anzahl))) == NULL) {
            perror("realloc");
            exit(EXIT_FAILURE);
        }

        /* String speichern */
        if ((r[(*anzahl) - 1] = strdup(token)) == NULL) {
            perror("strdup");
            exit(EXIT_FAILURE);
        }

        /* naechsten String holen */
        token = strtok(NULL, delims);
    }

    /* Speicher freigeben */
    free(kopie);

    /* Array zurueckgeben */
    return (r);
}

int main(int argc, char **argv)
{
    struct List *moeglich,      /* Liste der moeglichen Woerter des Anagramms */
    *anagramme,                 /* Liste alle Anagramme */
    *auswahl;                   /* Liste aller sinnvollen Anagramme */
    int i,                      /* Zaehlvariable */
     p,                         /* zum Auswerten der Parameter */
     max_depth;                 /* maximale Anzahl Woerter pro Anagramm */
    char *wort,                 /* Wort(gruppe), aus dem die Anagramme gebildet werden */
    *dict;                      /* das Woerterbuch, das benutzt werden soll */
    char **dateien;             /* Textdateien, die zum Herausfinden
                                   sinnvoller Anagramme verwendet werden */
    int anzahl;                 /* Anzahl der Textdateien */
    BOOL print,                 /* TRUE, wenn Zwischenergebnisse angezeigt werden sollen */
     streng;                    /* TRUE, wenn eine strenge Aussortierung erfolgen soll */

    /* Initialisierung */
    max_depth = 0;
    wort = NULL;
    dict = NULL;
    print = FALSE;
    streng = FALSE;
    dateien = NULL;

    printf("\n");

    /* Parameterauswertung */
    while ((p = getopt(argc, argv, "w:a:d:b:phs")) != -1) {
        switch (p) {
            /* Hilfe ausgeben */
        case 'h':
            print_usage();
            return (0);
            break;

            /* Wort(gruppe) einlesen */
        case 'w':
            if ((wort = strdup(optarg)) == NULL) {
                perror("strdup");
                exit(EXIT_FAILURE);
            }
            break;

            /* maximale Anzahl der Woerter pro Anagramm einlesen */
        case 'a':
            max_depth = atoi(optarg);
            break;

            /* pruefen, ob Zwischenergebnisse angezeigt werden sollen */
        case 'p':
            print = TRUE;
            break;

            /* Woerterbuch einlesen */
        case 'd':
            if ((dict = strdup(optarg)) == NULL) {
                perror("strdup");
                exit(EXIT_FAILURE);
            }
            break;

            /* pruefen, ob eine strenge Aussortierung erfolgen soll */
        case 's':
            streng = TRUE;
            break;

            /* Textdateien einlesen */
        case 'b':
            dateien = teilen(optarg, &anzahl);
            break;

            /* falsche Parameter angegeben */
        case '?':              /* Benutzer mitteilen, welcher Parameter falsch ist */
            if (optopt == 'w' || optopt == 'a'
                || optopt == 'd' || optopt == 'b')
                printf("\rOption -%c benoetigt ein Argument!\n", optopt);
            else
                printf("\rUnbekannte Option %c!\n", optopt);

            /* Hilfe ausgeben */
            print_usage();

            /* Programm beenden */
            return (0);
            break;
        }
    }

    /* pruefen, ob ein Wort angegeben wurde */
    if (wort == NULL) {
        /* Benutzer auf Fehler hinweisen */
        printf("\rKein Wort angegeben!\n");

        /* Hilfe ausgeben */
        print_usage();

        /* Programm beenden */
        return (0);
    }

    /* prufen, ob Textdateien eingegeben wurden */
    if (dateien == NULL) {
        /* Default-Wert festlegen */
        dateien = malloc(sizeof(char *));
        if ((dateien[0] = strdup(BUCH)) == NULL) {
            perror("strdup");
            exit(EXIT_FAILURE);
        }
        anzahl = 1;
    }

    /* pruefen, ob ein Woerterbuch angegeben wurde */
    if (dict == NULL)
        /* Default-Wert nehmen */
        if ((dict = strdup(DICT)) == NULL) {
            perror("strdup");
            exit(EXIT_FAILURE);
        }

    /* neue Liste erzeugen */
    anagramme = new_list();

    printf("Bitte warten.");
    fflush(stdout);

    /* moegliche Woerter fuer das Anagramm herausfinden */
    moeglich = finde_moegliche_woerter(dict, wort);

    /* wenn Zwischenergebnisse angezeigt werden sollen */
    if (print == TRUE) {
        printf("\rMoegliche Woerter fuer das Anagramm:\n");

        if (moeglich->count == 0) {
            /* Liste ist leer */
            printf("\rEs konnten keine Woerter gefunden werden,\n");
            printf("\raus denen man Anagramme bilden koennte\n");
        } else
            /* Liste der moeglichen Woerter fuer das Anagramm ausgeben */
            print_list(moeglich);

        printf("\n");
    }

    printf("\rBitte warten..");
    fflush(stdout);

    /* Anagramme finden und in Liste speichern */
    finde_anagramme(wort, moeglich, anagramme, NULL, 0, max_depth);

    /* wenn Zwischenergebnisse angezeigt werden sollen */
    if (print == TRUE) {
        printf("\rAlle Anagramme fuer das Wort %s:\n", wort);

        if (anagramme->count == 0)
            /* Liste ist leer */
            printf("\rEs konnten keine Anagramme gefunden werden\n");
        else
            /* Anagramme ausgeben */
            print_list(anagramme);
        printf("\n");
    }

    printf("\rBitte warten...");
    fflush(stdout);

    /* sinnvolle Anagramme herausfinden */
    auswahl = filter(moeglich, anagramme, dateien, anzahl, streng);

    printf("\rSinnvolle Anagramme fuer das Wort %s:\n", wort);

    if (auswahl->count == 0)
        /* keine sinnvollen Anagramme gefunden */
        printf("\rEs konnten keine sinnvollen Anagramme gefunden werden\n");
    else
        /* sinnvolle Anagramme ausgeben */
        print_list(auswahl);

    printf("\n");

    /* Speicher freigeben */
    for (i = 0; i < anzahl; i++)
        free(dateien[i]);
    free(dateien);
    delete_list(moeglich);
    delete_list(anagramme);
    delete_list(auswahl);
    free(wort);
    free(dict);

    /* Programm beenden */
    return (0);
}
