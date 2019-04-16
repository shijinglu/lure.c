#include<assert.h>
#include<stdlib.h>
#include<memory.h>
#include "node.h"
#include "data.h"
#include "function.h"
#include "util.h"
#include "hashmap.h"

/* evaluate, allocate and return a data struct. */
Data *node_function_evaluate(Node *node, map_t context) {
    LURE_ASSERT(node != NULL, "cannot evaluate against an empty node");
    LURE_ASSERT(node->left != NULL, "function name must not be NULL");
    LURE_ASSERT(node->list != NULL, "function parameter list must not be NULL");
    int n_params = node->list->n_all;
    Data **params = (Data **)calloc(n_params, sizeof(Data *));
    for(size_t i = 0; i < (size_t)n_params; i++) {
        Node *pnode = node->list->all[i];
        params[i] = pnode->evaluate(pnode, context);
    }
    Data *leftRes = node->left->evaluate(node->left, context);
    char *fname = leftRes->getCStr(leftRes);
    Data *ret = function_derive(fname, params, n_params);
    leftRes->clean(leftRes);
    free(leftRes);
    for(size_t i = 0; i < (size_t)n_params; i++) {
        params[i]->clean(params[i]);
        free(params[i]);
    }
    free(params);
    return ret;
}

Node *NewNodeFunction(const char *fname, NodeList *list) {
    Node *node = (Node *)calloc(1, sizeof(Node));
    node->type = NodeType_Function;
    node->data = NULL;
    node->left = NewNodeIdentity(fname);
    node->right = NULL;
    node->list = list;
    node->evaluate = node_function_evaluate;
    return node;
}
