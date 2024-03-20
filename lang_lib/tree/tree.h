#ifndef TREE_H_
#define TREE_H_

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

const int DEAD_SIZE = -1;

const char ADD = '+';
const char SUB = '-';
const char MUL = '*';
const char DIV = '/';
const char POW = '^';
const char EQ  = '=';
const char EXP = 'e';
const char SIN = 's';
const char COS = 'c';
const char PI  = 'p';
const char LN  = 'l';
const char SH  = 'w';
const char CH  = 'h';
typedef enum valueType
{
    VAR_TYPE = 1,
    NUM_TYPE = 2,
    OPER_TYPE = 3,
}valueType;

typedef struct value
{
    int type;
    double arg;
}value;
//const value DEAD_VALUE = {2003, '*', 24, (char )'A', 10};
typedef value *value_t;

typedef enum Errors
{
    TREE_IS_OK = 0,
    TREE_IS_NULL = 1 << 0,
    TREE_LOGER_ERROR = 1 << 1,
    TREE_ROOT_IS_NULL = 1 << 2,
    NODE_PTR_IS_NULL = 1 << 3,
    TRY_REMOVE_ROOT = 1 << 4,
    TRY_REMOVE_NONLEAF = 1 << 5,
    ADD_NODE_ERROR = 1 << 6,
}Errors;

typedef struct Node
{
    value_t value;

    Node* parent;
    Node* left;
    Node* right;
}Node;

typedef struct Tree
{
    FILE* log;
    Node* root;
    int size;
    int errors;
}Tree;

typedef enum WhichChild
{
    Right = 1,
    Left = 0,
}WhichChild;

void TreeCtor( Tree* tree );
void TreeDtor( Tree* tree );
int NodeDtor( Node* node );
Node* CopyNode( Node* data );
int InviteNode( Tree* tree, Node* node, WhichChild child, value_t value );
int RemoveNode( Tree* tree, Node* node );
int TreeDump( Tree* tree, FILE* logger );

#endif // #define TREE_H_