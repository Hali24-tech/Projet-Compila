#include "translator.h"
#include <stdio.h>
#include <string.h>

extern char symbol_table[][30];
extern int symbol_count;

static FILE *out = NULL;
static int indent = 0;

static void emit_indent(void){
    for(int i = 0; i < indent; i++) fprintf(out, "    ");
}

static int is_leaf(Node *n){
    return n && !n->child1 && !n->child2 && !n->child3;
}

static int is_binop(const char *op){
    return !strcmp(op, "+") || !strcmp(op, "-") || !strcmp(op, "*") || !strcmp(op, "/");
}

static void emit_expr(Node *e){
    if(!e) return;

    if(is_leaf(e)){
        fprintf(out, "%s", e->label);
        return;
    }

    if(is_binop(e->label)){
        fprintf(out, "(");
        emit_expr(e->child1);
        fprintf(out, " %s ", e->label);
        emit_expr(e->child2);
        fprintf(out, ")");
        return;
    }

    fprintf(out, "%s", e->label);
}

static void emit_cond(Node *c){
    emit_expr(c->child1);
    fprintf(out, " %s ", c->label);
    emit_expr(c->child2);
}

static void emit_inst(Node *n);

static void emit_block(Node *list){
    emit_inst(list);
}

static void emit_inst(Node *n){
    if(!n) return;

    if(!strcmp(n->label, "INST_LIST")){
        emit_inst(n->child1);
        emit_inst(n->child2);
        return;
    }

    if(!strcmp(n->label, "ASSIGN")){
        emit_indent();
        fprintf(out, "%s = ", n->child1->label);
        emit_expr(n->child2);
        fprintf(out, ";\n");
        return;
    }

    if(!strcmp(n->label, "READ")){
        emit_indent();
        fprintf(out, "scanf(\"%%d\", &%s);\n", n->child1->label);
        return;
    }

    if(!strcmp(n->label, "PRINT")){
        emit_indent();
        fprintf(out, "printf(\"%%d\\n\", ");
        emit_expr(n->child1);
        fprintf(out, ");\n");
        return;
    }

    if(!strcmp(n->label, "IF")){
        emit_indent();
        fprintf(out, "if (");
        emit_cond(n->child1);
        fprintf(out, ") {\n");
        indent++;
        emit_block(n->child2);
        indent--;
        emit_indent();
        fprintf(out, "}");

        if(n->child3){
            fprintf(out, " else {\n");
            indent++;
            emit_block(n->child3);
            indent--;
            emit_indent();
            fprintf(out, "}\n");
        } else {
            fprintf(out, "\n");
        }
        return;
    }

    if(!strcmp(n->label, "WHILE")){
        emit_indent();
        fprintf(out, "while (");
        emit_cond(n->child1);
        fprintf(out, ") {\n");
        indent++;
        emit_block(n->child2);
        indent--;
        emit_indent();
        fprintf(out, "}\n");
        return;
    }

    if(!strcmp(n->label, "FOR")){
        Node *id = n->child1;
        Node *start = n->child2;
        Node *tail = n->child3;
        Node *endExpr = tail->child1;
        Node *body = tail->child2;

        emit_indent();
        fprintf(out, "for (%s = ", id->label);
        emit_expr(start);
        fprintf(out, "; %s <= ", id->label);
        emit_expr(endExpr);
        fprintf(out, "; %s++) {\n", id->label);

        indent++;
        emit_block(body);
        indent--;
        emit_indent();
        fprintf(out, "}\n");
        return;
    }

    emit_indent();
    fprintf(out, "/* Non traduit: %s */\n", n->label);
}

void translate_to_c(Node *root){
    out = fopen("output.c", "w");
    if(!out){
        printf("Erreur: impossible de créer output.c\n");
        return;
    }

    fprintf(out, "#include <stdio.h>\n\n");
    fprintf(out, "int main(){\n");
    indent++;

    if(symbol_count > 0){
        emit_indent();
        fprintf(out, "int ");
        for(int i = 0; i < symbol_count; i++){
            fprintf(out, "%s", symbol_table[i]);
            if(i < symbol_count - 1) fprintf(out, ", ");
        }
        fprintf(out, ";\n\n");
    }

    if(root && !strcmp(root->label, "PROGRAM")){
        emit_inst(root->child1);
    }

    emit_indent();
    fprintf(out, "return 0;\n");
    indent--;
    fprintf(out, "}\n");

    fclose(out);
    out = NULL;
}
