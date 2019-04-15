#ifndef __LURE_NODE_H__
#define __LURE_NODE_H__

#include <stdbool.h>
#include "hashmap.h"
#include "data.h"

#define NODE_LIST_DEFAULT_CAPACITY 4
#define MAX_IDENTIFIER_LENGTH 256

typedef enum {
    BinOpType_EQ, 
    BinOpType_NE, 
    BinOpType_GT, 
    BinOpType_LT, 
    BinOpType_GE, 
    BinOpType_LE, 
    BinOpType_AND, 
    BinOpType_OR
} BinOpType;

typedef enum {
    NodeType_Unknown = 0,    /* Reducible nodes */
    NodeType_Empty,          /* A node that always produce false data. */
    NodeType_Function,       /* Function node execute a function and return result obj. */
    NodeType_BinOp,          /* Binary operator, <, <=, = , >=, >, && || */
    NodeType_UnaryOp,        /* Unary operator, not. */
    NodeType_Literal,        /* Literals, irreducible leafs.  */
    NodeType_Identity        /* Reducible leafs */
} NodeType;

typedef struct Node Node;

/* A simple array list that holds a list of Nodes*/
typedef struct NodeList {
    int capacity;
    /* a list of resolved data. */
    Node **all;
    int n_all;

    Node **unresolved; 
    int n_unresolved;

    /* map of key to Data from nonresolvable nodes. */
    map_t *resolved;
    /* if any node in the list is to be resolved. */
    bool   resolvable;
    
} NodeList;

/**
 * Add element to the list.
 * NOTE: this does not make copy of node or node->data.
 */
void node_list_add(NodeList *self, Node *node);
bool node_list_is_resolvable(NodeList *self);
bool node_list_in(NodeList *self, Data *data, map_t context);
NodeList *node_list_new(void);
void node_list_free(NodeList *list, bool deep);

struct Node {
    NodeType     type;      /*Type annotation. */
    Node        *left;      /*AST left node. */
    Node        *right;     /*AST right node. */
    NodeList    *list;      /*A list of nodes for Nodes like functions, in etc*/
    Data        *data;      /*Store associated data like string literal value. */
    BinOpType    op;        /*Operator type. */
    
    Data *(*evaluate)(Node *self, map_t context);
};

bool node_resolvable(Node *node);
void free_node_deep(Node *node);
Node *copy_node_deep(Node *node);

/* Constructors */
Node *NewNodeIdentity(const char *s);
Node *NewNodeBinOp(BinOpType op, Node *left, Node *right);
Node *NewNodeFunction(const char *fname, NodeList *list);
Node *NewNodeIn(Node *left, NodeList *list);
Node *NewNodeLike(Node *left, Node *right);
Node *NewNodeLiteral(Data *data);

/* helper methods to create literal nodes, will create new data object. */
Node *NewBooleanLiteral(bool val);
Node *NewIntLiteral(int val);
Node *NewDoubleLiteral(double val);
Node *NewStringLiteral(const char * val);

#endif
