#include "lexical.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

// ==============================
TSym_Cour SYM_COUR;
FILE *Fichier;
char Car_Cour;

// ==============================
void Lire_Car() {
    Car_Cour = fgetc(Fichier);
}

// ==============================
typedef struct {
    char nom[20];
    CODES_LEX code;
} MotCle;

MotCle table_mots[] = {
    {"start", START_TOKEN},
    {"stop", STOP_TOKEN},
    {"if", IF_TOKEN},
    {"then", THEN_TOKEN},
    {"end", END_TOKEN},
    {"while", WHILE_TOKEN},
    {"do", DO_TOKEN},
    {"read", READ_TOKEN},
    {"print", PRINT_TOKEN}
};

int NbMots = sizeof(table_mots) / sizeof(MotCle);

// ==============================
CODES_LEX Est_MotCle(char *mot) {
    char min[50];
    int i;

    for (i = 0; mot[i]; i++)
        min[i] = tolower(mot[i]);
    min[i] = '\0';

    for (i = 0; i < NbMots; i++)
        if (!strcmp(min, table_mots[i].nom))
            return table_mots[i].code;

    return ID_TOKEN;
}

// ==============================
void Passer_Separateurs() {
    while (isspace(Car_Cour))
        Lire_Car();
}

// ==============================
void Lire_Mot() {
    int i = 0;
    while (isalnum(Car_Cour)) {
        SYM_COUR.NOM[i++] = Car_Cour;
        Lire_Car();
    }
    SYM_COUR.NOM[i] = '\0';
    SYM_COUR.CODE = Est_MotCle(SYM_COUR.NOM);
}

// ==============================
void Lire_Nombre() {
    int i = 0;
    while (isdigit(Car_Cour)) {
        SYM_COUR.NOM[i++] = Car_Cour;
        Lire_Car();
    }
    SYM_COUR.NOM[i] = '\0';
    SYM_COUR.CODE = NUM_TOKEN;
}

// ==============================
void Sym_Suiv() {
    Passer_Separateurs();

    if (isalpha(Car_Cour)) {
        Lire_Mot();
    }
    else if (isdigit(Car_Cour)) {
        Lire_Nombre();
    }
    else {
        switch (Car_Cour) {

            case ':':
                Lire_Car();
                if (Car_Cour == '=') {
                    SYM_COUR.CODE = AFF_TOKEN;
                    strcpy(SYM_COUR.NOM, ":=");
                    Lire_Car();
                }
                break;

            case '+':
                SYM_COUR.CODE = PLUS_TOKEN;
                strcpy(SYM_COUR.NOM, "+");
                Lire_Car();
                break;

            case '<':
                SYM_COUR.CODE = INF_TOKEN;
                strcpy(SYM_COUR.NOM, "<");
                Lire_Car();
                break;

            case '>':
                SYM_COUR.CODE = SUP_TOKEN;
                strcpy(SYM_COUR.NOM, ">");
                Lire_Car();
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
}

void AfficherToken(TSym_Cour sym) {
    printf("<%d , %s>\n", sym.CODE, sym.NOM);
}

// --------------------------------------------------------------
// MAIN : exécution du LEXICAL SEUL
// --------------------------------------------------------------
/*int main() {
    Fichier = fopen("input.ml", "r");
    if (!Fichier) {
        printf("Erreur : impossible d'ouvrir input.ml\n");
        return 1;
    }

    Lire_Car();   // initialiser premier caractère

    do {
        Sym_Suiv();               // lire token suivant
        AfficherToken(SYM_COUR);  // afficher token
    } while (SYM_COUR.CODE != FIN_TOKEN);

    fclose(Fichier);
    return 0;
}
*/