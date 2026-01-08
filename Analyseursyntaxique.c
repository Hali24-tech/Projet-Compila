#include "lexical.h"
#include <stdio.h>
#include <stdlib.h>

// ==============================
void Erreur(const char *msg){
    printf("\n[ERREUR] %s\n", msg);
    printf("Symbole courant = %d | lexeme = '%s'\n",
           SYM_COUR.CODE, SYM_COUR.NOM);
    exit(1);
}

void Test(CODES_LEX attendu, const char *msg){
    if(SYM_COUR.CODE == attendu)
        Sym_Suiv();
    else
        Erreur(msg);
}

// ==============================
void PROGRAM();
void INST();
void EXPR();

// ==============================
void PROGRAM(){
    Test(START_TOKEN, "'start' attendu");

    while(SYM_COUR.CODE != STOP_TOKEN)
        INST();

    Test(STOP_TOKEN, "'stop' attendu");
}

// ==============================
void INST(){
    if(SYM_COUR.CODE == ID_TOKEN){
        Sym_Suiv();
        Test(AFF_TOKEN, "':=' attendu");
        EXPR();
        Test(PV_TOKEN, "';' attendu");
    }
    else if(SYM_COUR.CODE == READ_TOKEN){
        Sym_Suiv();
        Test(PO_TOKEN, "'(' attendu");
        Test(ID_TOKEN, "ID attendu");
        Test(PF_TOKEN, "')' attendu");
        Test(PV_TOKEN, "';' attendu");
    }
    else if(SYM_COUR.CODE == PRINT_TOKEN){
        Sym_Suiv();
        Test(PO_TOKEN, "'(' attendu");
        EXPR();
        Test(PF_TOKEN, "')' attendu");
        Test(PV_TOKEN, "';' attendu");
    }
    else {
        Erreur("Instruction inconnue");
    }
}

// ==============================
void EXPR(){
    if(SYM_COUR.CODE == ID_TOKEN || SYM_COUR.CODE == NUM_TOKEN)
        Sym_Suiv();
    else
        Erreur("Expression invalide");
}

// ==============================
int main(){
    Fichier = fopen("input.ml", "r");
    if(!Fichier){
        printf("Erreur ouverture fichier\n");
        return 1;
    }

    Lire_Car();
    Sym_Suiv();
    PROGRAM();

    printf("\n✅ BRAVO : programme syntaxiquement correct\n");
    fclose(Fichier);
    return 0;
}
