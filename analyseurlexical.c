#include "lexical.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

TSym_Cour SYM_COUR;
FILE *Fichier;
char Car_Cour;

void Lire_Car() {
    Car_Cour = (char)fgetc(Fichier);
}

typedef struct {
    char nom[20];
    CODES_LEX code;
} MotCle;

static MotCle table_mots[] = {
    {"start", START_TOKEN},
    {"stop",  STOP_TOKEN},
    {"if",    IF_TOKEN},
    {"then",  THEN_TOKEN},
    {"else",  ELSE_TOKEN},
    {"end",   END_TOKEN},
    {"while", WHILE_TOKEN},
    {"do",    DO_TOKEN},
    {"for",   FOR_TOKEN},
    {"to",    TO_TOKEN},
    {"read",  READ_TOKEN},
    {"print", PRINT_TOKEN}
};

static int NbMots = sizeof(table_mots) / sizeof(MotCle);

static CODES_LEX Est_MotCle(char *mot) {
    char min[50];
    int i;

    for (i = 0; mot[i]; i++)
        min[i] = (char)tolower((unsigned char)mot[i]);
    min[i] = '\0';

    for (i = 0; i < NbMots; i++)
        if (!strcmp(min, table_mots[i].nom))
            return table_mots[i].code;

    return ID_TOKEN;
}

static void Passer_Separateurs() {
    while (isspace((unsigned char)Car_Cour))
        Lire_Car();
}

static void Lire_Mot() {
    int i = 0;
    while (isalnum((unsigned char)Car_Cour)) {
        SYM_COUR.NOM[i++] = Car_Cour;
        Lire_Car();
    }
    SYM_COUR.NOM[i] = '\0';
    SYM_COUR.CODE = Est_MotCle(SYM_COUR.NOM);
}

static void Lire_Nombre() {
    int i = 0;
    while (isdigit((unsigned char)Car_Cour)) {
        SYM_COUR.NOM[i++] = Car_Cour;
        Lire_Car();
    }
    SYM_COUR.NOM[i] = '\0';
    SYM_COUR.CODE = NUM_TOKEN;
}

void Sym_Suiv() {
    Passer_Separateurs();

    if (isalpha((unsigned char)Car_Cour)) { Lire_Mot(); return; }
    if (isdigit((unsigned char)Car_Cour)) { Lire_Nombre(); return; }

    switch (Car_Cour) {

        case ':':
            Lire_Car();
            if (Car_Cour == '=') {
                SYM_COUR.CODE = AFF_TOKEN;
                strcpy(SYM_COUR.NOM, ":=");
                Lire_Car();
            } else {
                SYM_COUR.CODE = ERREUR_TOKEN;
                strcpy(SYM_COUR.NOM, ":");
            }
            break;

        case '+':
            SYM_COUR.CODE = PLUS_TOKEN;
            strcpy(SYM_COUR.NOM, "+");
            Lire_Car();
            break;

        case '-':
            SYM_COUR.CODE = MOINS_TOKEN;
            strcpy(SYM_COUR.NOM, "-");
            Lire_Car();
            break;

        case '*':
            SYM_COUR.CODE = MUL_TOKEN;
            strcpy(SYM_COUR.NOM, "*");
            Lire_Car();
            break;

        case '/':
            SYM_COUR.CODE = DIV_TOKEN;
            strcpy(SYM_COUR.NOM, "/");
            Lire_Car();
            break;

        case '<':
            Lire_Car();
            if (Car_Cour == '=') {
                SYM_COUR.CODE = INFEG_TOKEN;
                strcpy(SYM_COUR.NOM, "<=");
                Lire_Car();
            } else {
                SYM_COUR.CODE = INF_TOKEN;
                strcpy(SYM_COUR.NOM, "<");
            }
            break;

        case '>':
            Lire_Car();
            if (Car_Cour == '=') {
                SYM_COUR.CODE = SUPEG_TOKEN;
                strcpy(SYM_COUR.NOM, ">=");
                Lire_Car();
            } else {
                SYM_COUR.CODE = SUP_TOKEN;
                strcpy(SYM_COUR.NOM, ">");
            }
            break;

        case '=':
            Lire_Car();
            if (Car_Cour == '=') {
                SYM_COUR.CODE = EQ_TOKEN;
                strcpy(SYM_COUR.NOM, "==");
                Lire_Car();
            } else {
                SYM_COUR.CODE = ERREUR_TOKEN;
                strcpy(SYM_COUR.NOM, "=");
            }
            break;

        case '!':
            Lire_Car();
            if (Car_Cour == '=') {
                SYM_COUR.CODE = NEQ_TOKEN;
                strcpy(SYM_COUR.NOM, "!=");
                Lire_Car();
            } else {
                SYM_COUR.CODE = ERREUR_TOKEN;
                strcpy(SYM_COUR.NOM, "!");
            }
            break;

        case ';':
            SYM_COUR.CODE = PV_TOKEN;
            strcpy(SYM_COUR.NOM, ";");
            Lire_Car();
            break;

        case '(':
            SYM_COUR.CODE = PO_TOKEN;
            strcpy(SYM_COUR.NOM, "(");
            Lire_Car();
            break;

        case ')':
            SYM_COUR.CODE = PF_TOKEN;
            strcpy(SYM_COUR.NOM, ")");
            Lire_Car();
            break;

        case EOF:
            SYM_COUR.CODE = FIN_TOKEN;
            strcpy(SYM_COUR.NOM, "EOF");
            break;

        default:
            SYM_COUR.CODE = ERREUR_TOKEN;
            sprintf(SYM_COUR.NOM, "%c", Car_Cour);
            Lire_Car();
    }
}
/*
 * Commentaire explicatif général :
 *
 * Ce fichier implémente l’analyseur lexical du langage.
 * Son rôle principal est de lire le programme source caractère par caractère
 * et de le transformer en une suite de symboles lexicaux (tokens) compréhensibles
 * par l’analyseur syntaxique.
 *
 * L’analyseur lexical gère :
 *  - La lecture séquentielle des caractères depuis le fichier source.
 *  - L’élimination des séparateurs (espaces, tabulations, retours à la ligne).
 *  - La reconnaissance des mots-clés (start, if, while, etc.).
 *  - L’identification des identificateurs et des constantes numériques.
 *  - La détection des opérateurs arithmétiques, relationnels et symboles spéciaux.
 *
 * La fonction principale Sym_Suiv() permet d’obtenir, à chaque appel,
 * le prochain symbole lexical du programme et de le stocker dans la
 * variable globale SYM_COUR.
 *
 * Ce module constitue la première phase du compilateur et fournit
 * les éléments nécessaires à l’analyse syntaxique et à la construction
 * de l’arbre syntaxique abstrait (AST).
 */
