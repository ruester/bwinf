/* 30. Bundeswettbewerb Informatik
 * Aufgabe 4 - Zaras dritter Fehler
 *
 * Autor: Matthias Rüster
 *        Lena Kristin Zander
 *
 * E-Mail: ruester@molgen.mpg.de
 *         zander@molgen.mpg.de
 */

#include <stdio.h>
#include <ctype.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wchar.h>
#include <wctype.h>

#include <locale.h>
#include <errno.h>

#include "hash.h"

#define BOOK "EffiBriest.txt"
#define DICT "igerman98.dict"

struct code {
    int *nr;
    int count;
};

struct book {
    wchar_t *line; /* alle Buchstaben des Textes hintereinander */
    int chars;    /* Anzahl der eingelesenen Buchstaben */
    int *a;       /* Merken der "newlines" */
    int count;    /* Groesse des Arrays a */
};

struct sentence {
    double value;
    wchar_t *s;
};

void usage(void)
{
    wprintf(L"Verwendung: aufgabe4 [Optionen] Dateiname...\n\n"

            L"moegliche Optionen:\n"
            L"    -p Anzahl         zusaetzliche Informationen ausgeben\n"
            L"    -b Buch           Buch festlegen\n"
            L"    -d Woerterbuch    Woerterbuch festlegen\n"
            L"    -h                diese Hilfe anzeigen\n");
}

void lowercase(wchar_t *s)
{
    while (*s) {
        *s = towlower(*s);
        s++;
    }
}

/*
 * liest die Zahlenfolge aus einer Datei ein
 */
void read_code(char *filename, struct code *c)
{
    FILE *f;
    int i;
    char line[LINE_MAX];
    char *number;

    if ((f = fopen(filename, "r")) == NULL) {
        perror("read_code: fopen");
        exit(EXIT_FAILURE);
    }

    if (fgets(line, LINE_MAX, f) == NULL) {
        fwprintf(stderr, L"Zahlenfolge konnte nicht eingelesen werden\n");
        exit(EXIT_FAILURE);
    }

    fclose(f);

    if ((c->nr = calloc(1, sizeof(*(c->nr)))) == NULL) {
        perror("read_code: calloc");
        exit(EXIT_FAILURE);
    }

    c->nr[0] = atoi(strtok(line, " "));

    c->count = i = 1;

    while ((number = strtok(NULL, " ")) != NULL) {
        c->nr = realloc(c->nr, (c->count + 1) * sizeof(*(c->nr)));

        if (!c->nr) {
            perror("read_code: realloc");
            exit(EXIT_FAILURE);
        }

        c->nr[i] = atof(number);
        i++;
        c->count++;
    }
}

/*
 * liest alle Zeichen einer Textdatei ein
 */
void read_book(char *filename, struct book *b)
{
    FILE *f;
    char newline;
    wint_t c;
    int i;

    if ((f = fopen(filename, "r")) == NULL) {
        perror("read_book: fopen");
        exit(EXIT_FAILURE);
    }

    b->count = b->chars = i = 0;
    b->a     = NULL;
    b->line  = NULL;
    newline  = 1;

    errno = 0;

    while ((c = fgetwc(f)) != WEOF) {
        if (c == '\n')
            newline = 1;

        if (iswalpha(c)) {
            if (newline == 1) {
                b->a = realloc(b->a, (b->count + 1) * sizeof(*(b->a)));

                if (!b->a) {
                    perror("read_book: realloc");
                    exit(EXIT_FAILURE);
                }

                b->a[b->count] = b->chars;
                newline = 0;
                b->count++;
            }

            b->line = realloc(b->line, (b->chars + 1) * sizeof(*(b->line)));

            if (!b->line) {
                perror("read_book: realloc");
                exit(EXIT_FAILURE);
            }

            b->line[b->chars] = c;
            b->chars++;
        }
    }

    if (errno == EILSEQ) {
        fwprintf(stderr, L"read_book: UTF-8 convert error\n"
                         L"try: export LC_ALL=de_DE.UTF8\n");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < b->chars; i++)
        b->line[i] = towlower(b->line[i]);

    fclose(f);
}

/*
 * entschlusselt die Zahlenfolge und
 * erstellt ein Array von code-Strukturen,
 * die alle moeglichen Loesungen beinhalten
 */
struct sentence *decrypt(struct book *b, struct code *c, int *count)
{
    int sum, cnt, i, j;
    struct sentence *s;

    *count = 0;

    if ((s = calloc(b->count, sizeof (*s))) == NULL) {
        perror("decrypt: calloc");
        exit(EXIT_FAILURE);
    }

    sum = 0;
    for (j = 0; j < c->count; j++)
        sum += c->nr[j];

    for (i = 0; i < b->count; i++) {
        cnt = b->a[i] - 1;

        if (cnt + sum >= b->chars)
            break;

        s[*count].s = calloc(c->count + 1, sizeof(*(s[*count].s)));

        if (!s[*count].s) {
            perror("decrypt: calloc");
            exit(EXIT_FAILURE);
        }

        for (j = 0; j < c->count; j++){
            cnt            += c->nr[j];
            s[*count].s[j]  = b->line[cnt];
        }

        (*count)++;
    }

    return s;
}

/*
 * sortiert das Array der moeglichen Loesungen nach
 * ihrem Wahrscheinlichkeitswert (absteigend)
 */
void sort(struct sentence *array, int num)
{
    int swapped = 1;
    int i;
    struct sentence temp;

    while (swapped > 0) {
        swapped = 0;

        for (i = 0; i < num - 1 ; i++) {
            if (array[i].value < array[i+1].value) {
                temp       = array[i];
                array[i]   = array[i+1];
                array[i+1] = temp;
                swapped++;
            }
        }
    }
}

/*
 * liest alle Woerter eines Woerterbuchs ein
 * und speichert diese in einem Hash
 */
struct hash *read_dictionary(char *filename)
{
    struct hash *h;
    wchar_t line[LINE_MAX] = {0};
    FILE *f;
    int i;

    h = hash_new();

    if ((f = fopen(filename, "r")) == NULL) {
        perror("read_dictionary: fopen");
        exit(EXIT_FAILURE);
    }

    i = errno = 0;

    while (fgetws(line, LINE_MAX, f) != NULL) {
        line[wcslen(line) - 1] = '\0';
        lowercase(line);

        if (!hash_search(h, line))
            hash_insert(h, entry_new(line));
    }

    if (errno == EILSEQ) {
        fwprintf(stderr, L"read_dictionary: UTF-8 convert error\n"
                         L"try: export LC_ALL=de_DE.UTF8\n");
        exit(EXIT_FAILURE);
    }

    fclose(f);

    return h;
}

/*
 * Loesungssatz mit Hilfe des Woerterbuches bewerten
 */
void dict_search(struct hash *h, struct sentence *s)
{
    wchar_t a[LINE_MAX];
    char matched[LINE_MAX] = {0};
    int i, j, l, m, k;

    l = wcslen(s->s);

    for (i = 0; i < l - 1; i++) {
        for (j = i; j < l; j++) {
            k = j - i + 1;

            memset(a, 0, LINE_MAX * sizeof(*a));
            memcpy(a, s->s + i, k * sizeof(*a));

            if (hash_search(h, a))
                memset(matched + i, 1, k * sizeof(*matched));
        }
    }

    m = 0;
    for (i = 0; i < l; i++)
        if (matched[i])
            m++;

    s->value = ((double) m / (double) l) * 100.0;
}

/*
 * rekursive Funktion zum Ausgeben aller moeglichen Klartexte
 */
char get_plaintext(wchar_t *s, struct hash *dict,
                   int start, wchar_t **e, int *words)
{
    char r;
    int i, l, k;
    wchar_t w[LINE_MAX];

    l = wcslen(s);
    r = 0;

    if (start == l) {
        for (i = 0; i < *words; i++)
            wprintf(L"%ls ", e[i]);
        wprintf(L"\n");

        return 1;
    }

    for (i = start + 1; i < l; i++) {
        k = i - start + 1;

        memcpy(w, s + start, sizeof(*w) * k);
        w[k] = '\0';

        if (hash_search(dict, w)) {
            memcpy(e[*words], w, sizeof(*e) * k);

            (*words)++;

            if (get_plaintext(s, dict, i + 1, e, words))
                r = 1;

            (*words)--;
        }
    }

    return r;
}

/*
 * sucht alle moeglichen Klartexte und gibt diese auf den Bildschirm aus
 */
void print_plaintext(wchar_t *s, struct hash *dict)
{
    wchar_t **e;
    int i, l, words;

    l = wcslen(s);

    if ((e = calloc(l, sizeof(*e))) == NULL) {
        perror("print_plaintext: calloc");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < l; i++) {
        if ((e[i] = calloc(l + 1, sizeof(*(e[0])))) == NULL) {
            perror("print_plaintext: calloc");
            exit(EXIT_FAILURE);
        }
    }

    wprintf(L"Klartext(e):\n");

    words = 0;

    if (!get_plaintext(s, dict, 0, e, &words))
        wprintf(L"Es konnte kein Klartext ermittelt werden\n");

    for (i = 0; i < l; i++)
        free(e[i]);
    free(e);
}

int main(int argc, char *argv[])
{
    char *dictfile, *bookfile;
    struct code code;
    struct book book;
    struct sentence *sentence;
    int i, j, count, opt, more;
    struct hash *dict;

    if (argc < 2) {
        wprintf(L"Dateiname der Zahlenfolge als Parameter benoetigt\n");
        return EXIT_FAILURE;
    }

    setlocale(LC_ALL, "");

    /* Standardwerte */
    dictfile = DICT;
    bookfile = BOOK;
    more     = 0;

    while ((opt = getopt(argc, argv, "hp:d:b:")) != -1) {
        switch (opt) {
            case 'h':
                usage();
                return EXIT_FAILURE;

            case 'p':
                if ((more = atoi(optarg)) <= 0) {
                    fwprintf(stderr, L"Anzahl der Saetze muss groesser 0 sein");
                    return EXIT_FAILURE;
                }
                break;

            case 'd':
                dictfile = optarg;
                break;

            case 'b':
                bookfile = optarg;
                break;

            default:
                if (optopt == 'd' || optopt == 'b')
                    fwprintf(stderr, L"Option -%c benoetigt ein Argument\n",
                            optopt);
                else
                    fwprintf(stderr, L"unbekannte Option -%c\n", optopt);

                usage();
                return EXIT_FAILURE;
        }
    }

    if (optind == argc) {
        fwprintf(stderr,
                 L"Dateiname der Zahlenfolge als Parameter benoetigt\n");
        return EXIT_FAILURE;
    }

    /* Buch einlesen */
    read_book(bookfile, &book);

    /* Woerterbuch einlesen */
    dict = read_dictionary(dictfile);

    for (j = optind; j < argc; j++) {
        if (optind + 1 != argc)
            wprintf(L"%s\n", argv[j]);

        /* Zahlenfolge einlesen */
        read_code(argv[j], &code);

        /* Zahlenfolge entschluesseln */
        sentence = decrypt(&book, &code, &count);

        /* Woerterbuch durchsuchen */
        for (i = 0; i < count; i++)
            dict_search(dict, &(sentence[i]));

        /* Saetze sortieren */
        sort(sentence, count);

        if (more) {
            for (i = 0; i < more && i < count; i++)
                wprintf(L"%6.1f %%    %ls\n",
                        sentence[i].value, sentence[i].s);
        } else {
            wprintf(L"entschluesselt: %ls\n", sentence[0].s);

            if (sentence[0].value == 100.0)
                print_plaintext(sentence[0].s, dict);
        }

        for (i = 0; i < count; i++)
            free(sentence[i].s);
        free(sentence);

        if (optind + 1 != argc && j != argc - 1)
            wprintf(L"\n");

        free(code.nr);
    }

    free(book.line);
    free(book.a);
    hash_free(dict);

    return EXIT_SUCCESS;
}
