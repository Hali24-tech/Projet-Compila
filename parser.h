#ifndef PARSER_H
#define PARSER_H

typedef struct Node {
    char label[30];
    struct Node *child1;
    struct Node *child2;
    struct Node *child3;
} Node;

#endif
