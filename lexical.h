#ifndef LEXICAL_H
#define LEXICAL_H

#include <stdio.h>

// ==============================
// Codes lexicaux (TOKENS)
// ==============================
typedef enum {
    START_TOKEN,
    STOP_TOKEN,

    IF_TOKEN,
    THEN_TOKEN,
    END_TOKEN,

    WHILE_TOKEN,
    DO_TOKEN,

    READ_TOKEN,
    PRINT_TOKEN,

    ID_TOKEN,
    NUM_TOKEN,

    AFF_TOKEN,     // :=
    PLUS_TOKEN,    // +
    INF_TOKEN,     // <
    SUP_TOKEN,     // >

    PV_TOKEN,      // ;
    PO_TOKEN,      // (
    PF_TOKEN,      // )

    FIN_TOKEN,
    ERREUR_TOKEN
} CODES_LEX;

// ==============================
// Symbole courant
// ==============================
typedef struct {
    CODES_LEX CODE;
    char NOM[50];
} TSym_Cour;

// ==============================
// Variables globales partagées
// ==============================
extern TSym_Cour SYM_COUR;
extern FILE *Fichier;

// ==============================
// Fonctions lexicales
// ==============================
void Lire_Car(void);
void Sym_Suiv(void);

#endif
