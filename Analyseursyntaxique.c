#include "lexical.h"
#include "parser.h"
#include "translator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_VARS 100

char symbol_table[MAX_VARS][30];
int symbol_count = 0;

void add_variable(const char *name){
    for(int i = 0; i < symbol_count; i++){
        if(strcmp(symbol_table[i], name) == 0) return;
    }
    if(symbol_count < MAX_VARS) strcpy(symbol_table[symbol_count++], name);
    else { printf("[ERREUR SEMANTIQUE] Trop de variables.\n"); exit(1); }
}

int exists_variable(const char *name){
    for(int i = 0; i < symbol_count; i++)
        if(strcmp(symbol_table[i], name) == 0) return 1;
    return 0;
}

Node* newNode(const char *label, Node *c1, Node *c2, Node *c3){
    Node *n = (Node*)malloc(sizeof(Node));
    strcpy(n->label, label);
    n->child1 = c1; n->child2 = c2; n->child3 = c3;
    return n;
}

void Erreur(const char *msg){
    printf("\n[ERREUR SYNTAXIQUE] %s\n", msg);
    printf("Symbole courant = %d | lexeme = '%s'\n", SYM_COUR.CODE, SYM_COUR.NOM);
    exit(1);
}

void Test(CODES_LEX attendu, const char *msg){
    if(SYM_COUR.CODE == attendu) Sym_Suiv();
    else Erreur(msg);
}

/* Prototypes */
Node* PROGRAM();
Node* INST();
Node* BLOCK();
Node* COND();
Node* EXPR();
Node* TERM();
Node* FACT();

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
BLOCK → { INST } s'arrête sur ELSE ou END (sans consommer)
*/
Node* BLOCK(){
    Node *liste = NULL;
    while(SYM_COUR.CODE != END_TOKEN && SYM_COUR.CODE != ELSE_TOKEN){
        Node *inst = INST();
        liste = newNode("INST_LIST", liste, inst, NULL);
    }
    return liste;
}

/*
COND → EXPR RELOP EXPR
*/
Node* COND(){
    Node *g = EXPR();

    if(SYM_COUR.CODE == INF_TOKEN){ Sym_Suiv(); return newNode("<",  g, EXPR(), NULL); }
    if(SYM_COUR.CODE == SUP_TOKEN){ Sym_Suiv(); return newNode(">",  g, EXPR(), NULL); }
    if(SYM_COUR.CODE == INFEG_TOKEN){ Sym_Suiv(); return newNode("<=", g, EXPR(), NULL); }
    if(SYM_COUR.CODE == SUPEG_TOKEN){ Sym_Suiv(); return newNode(">=", g, EXPR(), NULL); }
    if(SYM_COUR.CODE == EQ_TOKEN){ Sym_Suiv(); return newNode("==", g, EXPR(), NULL); }
    if(SYM_COUR.CODE == NEQ_TOKEN){ Sym_Suiv(); return newNode("!=", g, EXPR(), NULL); }

    Erreur("Opérateur de comparaison attendu (<, >, <=, >=, ==, !=)");
    return NULL;
}

/*
EXPR → TERM { (+|-) TERM }
TERM → FACT { (*|/) FACT }
FACT → ID | NUM | '(' EXPR ')'
*/
Node* EXPR(){
    Node *left = TERM();
    while(SYM_COUR.CODE == PLUS_TOKEN || SYM_COUR.CODE == MOINS_TOKEN){
        CODES_LEX op = SYM_COUR.CODE;
        Sym_Suiv();
        Node *right = TERM();
        left = newNode(op == PLUS_TOKEN ? "+" : "-", left, right, NULL);
    }
    return left;
}

Node* TERM(){
    Node *left = FACT();
    while(SYM_COUR.CODE == MUL_TOKEN || SYM_COUR.CODE == DIV_TOKEN){
        CODES_LEX op = SYM_COUR.CODE;
        Sym_Suiv();
        Node *right = FACT();
        left = newNode(op == MUL_TOKEN ? "*" : "/", left, right, NULL);
    }
    return left;
}

Node* FACT(){
    if(SYM_COUR.CODE == ID_TOKEN){
        if(!exists_variable(SYM_COUR.NOM)){
            printf("[ERREUR SEMANTIQUE] Variable '%s' utilisée avant déclaration.\n", SYM_COUR.NOM);
            exit(1);
        }
        Node *n = newNode(SYM_COUR.NOM, NULL, NULL, NULL);
        Sym_Suiv();
        return n;
    }
    if(SYM_COUR.CODE == NUM_TOKEN){
        Node *n = newNode(SYM_COUR.NOM, NULL, NULL, NULL);
        Sym_Suiv();
        return n;
    }
    if(SYM_COUR.CODE == PO_TOKEN){
        Sym_Suiv();
        Node *e = EXPR();
        Test(PF_TOKEN, "')' attendu");
        return e;
    }
    Erreur("FACT invalide (ID, NUM ou (EXPR) attendu)");
    return NULL;
}

/*
INST →
   ID := EXPR ;
 | read ( ID ) ;
 | print ( EXPR ) ;
 | if COND then BLOCK [else BLOCK] end
 | while COND do BLOCK end
 | for ID := EXPR to EXPR do BLOCK end
*/
Node* INST(){

    if(SYM_COUR.CODE == ID_TOKEN){
        char id[30];
        strcpy(id, SYM_COUR.NOM);
        Sym_Suiv();

        Test(AFF_TOKEN, "':=' attendu");
        Node *expr = EXPR();
        Test(PV_TOKEN, "';' attendu");

        add_variable(id);
        return newNode("ASSIGN", newNode(id, NULL, NULL, NULL), expr, NULL);
    }

    if(SYM_COUR.CODE == READ_TOKEN){
        Sym_Suiv();
        Test(PO_TOKEN, "'(' attendu");

        char id[30];
        strcpy(id, SYM_COUR.NOM);
        Test(ID_TOKEN, "ID attendu");

        add_variable(id);

        Test(PF_TOKEN, "')' attendu");
        Test(PV_TOKEN, "';' attendu");

        return newNode("READ", newNode(id, NULL, NULL, NULL), NULL, NULL);
    }

    if(SYM_COUR.CODE == PRINT_TOKEN){
        Sym_Suiv();
        Test(PO_TOKEN, "'(' attendu");
        Node *expr = EXPR();
        Test(PF_TOKEN, "')' attendu");
        Test(PV_TOKEN, "';' attendu");
        return newNode("PRINT", expr, NULL, NULL);
    }

    if(SYM_COUR.CODE == IF_TOKEN){
        Sym_Suiv();
        Node *cond = COND();
        Test(THEN_TOKEN, "'then' attendu");

        Node *thenBlock = BLOCK();
        Node *elseBlock = NULL;

        if(SYM_COUR.CODE == ELSE_TOKEN){
            Sym_Suiv();
            elseBlock = BLOCK();
        }

        Test(END_TOKEN, "'end' attendu");
        return newNode("IF", cond, thenBlock, elseBlock);
    }

    if(SYM_COUR.CODE == WHILE_TOKEN){
        Sym_Suiv();
        Node *cond = COND();
        Test(DO_TOKEN, "'do' attendu");

        Node *body = BLOCK();
        Test(END_TOKEN, "'end' attendu");

        return newNode("WHILE", cond, body, NULL);
    }

    if(SYM_COUR.CODE == FOR_TOKEN){
    Sym_Suiv();

    char id[30];
    strcpy(id, SYM_COUR.NOM);
    Test(ID_TOKEN, "ID attendu");

    // ✅ IMPORTANT : déclarer la variable de boucle AVANT de lire les expressions
    add_variable(id);

    Test(AFF_TOKEN, "':=' attendu");
    Node *start = EXPR();

    Test(TO_TOKEN, "'to' attendu");
    Node *endExpr = EXPR();

    Test(DO_TOKEN, "'do' attendu");
    Node *body = BLOCK();
    Test(END_TOKEN, "'end' attendu");

    Node *tail = newNode("FOR_TAIL", endExpr, body, NULL);
    return newNode("FOR", newNode(id, NULL, NULL, NULL), start, tail);
}


    Erreur("Instruction inconnue");
    return NULL;
}

void afficher(Node *n, int niveau){
    if(!n) return;
    for(int i = 0; i < niveau; i++) printf("  ");
    printf("%s\n", n->label);
    afficher(n->child1, niveau + 1);
    afficher(n->child2, niveau + 1);
    afficher(n->child3, niveau + 1);
}
int main(int argc, char *argv[]){
    if(argc < 2){
        printf("Usage: %s <fichier_pseudo_code>\n", argv[0]);
        return 1;
    }

    Fichier = fopen(argv[1], "r");
    if(!Fichier){
        printf("❌ Erreur : impossible d'ouvrir %s\n", argv[1]);
        return 1;
    }

    printf("=====================================\n");
    printf("        PSEUDO-CODE ANALYZER\n");
    printf("=====================================\n");
    printf("Fichier analyse : %s\n\n", argv[1]);

    Lire_Car();
    Sym_Suiv();

    Node *arbre = PROGRAM();

    printf("\n [OK] Analyse lexicale terminee");
    printf("\n [OK] Analyse syntaxique terminee");
    printf("\n [OK] Analyse semantique terminee\n");

    printf("\n====== PARSE TREE ======\n");
    afficher(arbre, 0);

    translate_to_c(arbre);
    printf("\n✅ Fichier généré : output.c\n");

    fclose(Fichier);
    return 0;
}


// int main(){
//     Fichier = fopen("input.ml", "r");
//     if(!Fichier){
//         printf("Erreur ouverture fichier\n");
//         return 1;
//     }

//     Lire_Car();
//     Sym_Suiv();

//     Node *arbre = PROGRAM();

//     printf("\n====== PARSE TREE ======\n");
//     afficher(arbre, 0);

//     translate_to_c(arbre);
//     printf("\n✅ Fichier généré : output.c\n");

//     fclose(Fichier);
//     return 0;
// }
/*
 * Commentaire explicatif général :
 *
 * Ce fichier implémente l’analyseur syntaxique (parser) du langage MiniLang ainsi qu’une
 * vérification sémantique simple liée aux variables. Il s’appuie sur l’analyseur lexical
 * (lexical.c) pour récupérer les tokens, puis construit un Arbre Syntaxique Abstrait (AST)
 * à l’aide de la structure Node définie dans parser.h.
 *
 * 1) Gestion des symboles (table des variables)
 *    - symbol_table, add_variable() et exists_variable() permettent d’enregistrer les variables
 *      rencontrées (déclaration implicite via read ou affectation) et de détecter l’utilisation
 *      d’une variable avant sa déclaration.
 *
 * 2) Construction de l’AST
 *    - newNode() crée un nœud AST avec un label et jusqu’à trois enfants.
 *    - PROGRAM(), INST(), BLOCK(), COND(), EXPR(), TERM(), FACT() sont des fonctions récursives
 *      qui appliquent la grammaire du langage (analyse descendante récursive).
 *
 * 3) Contrôle syntaxique et messages d’erreur
 *    - Test() vérifie qu’un token attendu est bien présent, sinon Erreur() affiche un message
 *      détaillé indiquant le symbole courant (code + lexème) avant d’arrêter le programme.
 *
 * 4) Affichage et traduction
 *    - afficher() affiche l’arbre obtenu (parse tree / AST) pour faciliter le débogage.
 *    - translate_to_c(arbre) (dans translator.c) parcourt l’AST et génère un fichier output.c.
 *
 * 5) Fonction main()
 *    - Ouvre le fichier source "input.ml", initialise la lecture lexicale, lance PROGRAM()
 *      pour construire l’AST, affiche l’arbre, puis génère le code C final.
 *
 * Ce module représente la phase d’analyse syntaxique du compilateur et produit une structure
 * (AST) exploitable par l’étape suivante : la traduction/génération de code en langage C.
 */
