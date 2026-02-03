#ifndef LEXICAL_H
#define LEXICAL_H

#include <stdio.h>

typedef enum {
    START_TOKEN,
    STOP_TOKEN,

    IF_TOKEN,
    THEN_TOKEN,
    ELSE_TOKEN,
    END_TOKEN,

    WHILE_TOKEN,
    DO_TOKEN,

    FOR_TOKEN,
    TO_TOKEN,

    READ_TOKEN,
    PRINT_TOKEN,

    ID_TOKEN,
    NUM_TOKEN,

    AFF_TOKEN,    // :=
    PLUS_TOKEN,   // +
    MOINS_TOKEN,  // -
    MUL_TOKEN,    // *
    DIV_TOKEN,    // /

    INF_TOKEN,    // <
    SUP_TOKEN,    // >
    INFEG_TOKEN,  // <=
    SUPEG_TOKEN,  // >=
    EQ_TOKEN,     // ==
    NEQ_TOKEN,    // !=

    PV_TOKEN,     // ;
    PO_TOKEN,     // (
    PF_TOKEN,     // )

    FIN_TOKEN,
    ERREUR_TOKEN
} CODES_LEX;

typedef struct {
    CODES_LEX CODE;
    char NOM[50];
} TSym_Cour;

extern TSym_Cour SYM_COUR;
extern FILE *Fichier;

void Lire_Car(void);
void Sym_Suiv(void);

#endif
/*
 * Commentaire explicatif général :
 *
 * Le fichier lexical.h définit les structures, types et constantes nécessaires
 * au fonctionnement de l’analyseur lexical.
 *
 * Il contient :
 *  - L’énumération CODES_LEX, qui regroupe l’ensemble des codes lexicaux
 *    représentant les mots-clés, identificateurs, constantes, opérateurs
 *    et symboles spéciaux du langage.
 *
 *  - La structure TSym_Cour, utilisée pour stocker le symbole lexical courant.
 *    Elle associe le code lexical du symbole (CODE) à sa représentation
 *    textuelle (NOM).
 *
 *  - Les déclarations des variables globales SYM_COUR et Fichier, partagées
 *    entre l’analyseur lexical et les autres modules du compilateur.
 *
 *  - Les prototypes des fonctions principales Lire_Car() et Sym_Suiv(),
 *    permettant respectivement de lire un caractère depuis le fichier source
 *    et de produire le prochain symbole lexical.
 *
 * Ce fichier joue le rôle d’interface entre l’analyseur lexical et les autres
 * phases du compilateur, en garantissant une communication claire et structurée
 * des informations lexicales.
 */
