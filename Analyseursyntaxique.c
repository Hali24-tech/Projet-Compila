#include "lexical.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ========================================================= */
/* ================== STRUCTURE DU PARSE TREE =============== */
/* ========================================================= */

typedef struct Node {
    char label[30];
    struct Node *child1;
    struct Node *child2;
    struct Node *child3;
} Node;

Node* newNode(const char *label, Node *c1, Node *c2, Node *c3){
    Node *n = (Node*)malloc(sizeof(Node));
    strcpy(n->label, label);
    n->child1 = c1;
    n->child2 = c2;
    n->child3 = c3;
    return n;
}

/* ========================================================= */
/* ====================== ERREURS =========================== */
/* ========================================================= */

void Erreur(const char *msg){
    printf("\n[ERREUR SYNTAXIQUE] %s\n", msg);
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

/* ========================================================= */
/* ==================== PROTOTYPES ========================== */
/* ========================================================= */

Node* PROGRAM();
Node* INST();
Node* EXPR();

/* ========================================================= */
/* ==================== GRAMMAIRE =========================== */
/* ========================================================= */

/*
PROGRAM → start { INST } stop
*/

Node* PROGRAM(){
    Test(START_TOKEN, "'start' attendu");

    Node *liste = NULL;

    while(SYM_COUR.CODE != STOP_TOKEN){
        Node *inst = INST();
        liste = newNode("INST_LIST", liste, inst, NULL);
    }

    Test(STOP_TOKEN, "'stop' attendu");

    return newNode("PROGRAM", liste, NULL, NULL);
}

/*
INST →
     ID := EXPR ;
   | read ( ID ) ;
   | print ( EXPR ) ;
*/

Node* INST(){

    /* Affectation */
    if(SYM_COUR.CODE == ID_TOKEN){
        char id[30];
        strcpy(id, SYM_COUR.NOM);
        Sym_Suiv();

        Test(AFF_TOKEN, "':=' attendu");

        Node *expr = EXPR();

        Test(PV_TOKEN, "';' attendu");

        return newNode("ASSIGN",
                newNode(id, NULL, NULL, NULL),
                expr,
                NULL);
    }

    /* read */
    else if(SYM_COUR.CODE == READ_TOKEN){
        Sym_Suiv();

        Test(PO_TOKEN, "'(' attendu");

        char id[30];
        strcpy(id, SYM_COUR.NOM);
        Test(ID_TOKEN, "ID attendu");

        Test(PF_TOKEN, "')' attendu");
        Test(PV_TOKEN, "';' attendu");

        return newNode("READ",
                newNode(id, NULL, NULL, NULL),
                NULL,
                NULL);
    }

    /* print */
    else if(SYM_COUR.CODE == PRINT_TOKEN){
        Sym_Suiv();

        Test(PO_TOKEN, "'(' attendu");
        Node *expr = EXPR();
        Test(PF_TOKEN, "')' attendu");
        Test(PV_TOKEN, "';' attendu");

        return newNode("PRINT", expr, NULL, NULL);
    }

    else{
        Erreur("Instruction inconnue");
        return NULL;
    }
}

/*
EXPR → ID | NUM
*/

Node* EXPR(){
    if(SYM_COUR.CODE == ID_TOKEN){
        Node *n = newNode(SYM_COUR.NOM, NULL, NULL, NULL);
        Sym_Suiv();
        return n;
    }
    else if(SYM_COUR.CODE == NUM_TOKEN){
        Node *n = newNode(SYM_COUR.NOM, NULL, NULL, NULL);
        Sym_Suiv();
        return n;
    }
    else{
        Erreur("Expression invalide");
        return NULL;
    }
}

/* ========================================================= */
/* ==================== AFFICHAGE ARBRE ===================== */
/* ========================================================= */

void afficher(Node *n, int niveau){
    if(!n) return;

    for(int i = 0; i < niveau; i++)
        printf("  ");

    printf("%s\n", n->label);

    afficher(n->child1, niveau + 1);
    afficher(n->child2, niveau + 1);
    afficher(n->child3, niveau + 1);
}

/* ========================================================= */
/* ========================= MAIN =========================== */
/* ========================================================= */

int main(){
    Fichier = fopen("input.ml", "r");
    if(!Fichier){
        printf("Erreur ouverture fichier\n");
        return 1;
    }

    Lire_Car();
    Sym_Suiv();

    Node *arbre = PROGRAM();

    printf("\n====== PARSE TREE ======\n");
    afficher(arbre, 0);

    fclose(Fichier);
    return 0;
}
