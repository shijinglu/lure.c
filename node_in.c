#include<assert.h>
#include<stdlib.h>
#include<memory.h>
#include "node.h"
#include "data.h"
#include "util.h"
#include "hashmap.h"

/* evaluate, allocate and return a data struct. */
Data *node_in_evaluate(Node *node, map_t context) {
    LURE_ASSERT(node != NULL, "cannot evaluate against an empty node");
    LURE_ASSERT(node->left != NULL, "left side of an <in> operation must not be NULL");
    LURE_ASSERT(node->list != NULL, "list of an <in> node must not be NULL");
    if (node->list->n_all == 0) {
        return NewBoolData(false);
    }
    Data *leftRes = node->left->evaluate(node->left, context);
    bool flag = node_list_in(node->list, leftRes, context);
    leftRes->clean(leftRes); free(leftRes);
    return flag ? NewBoolData(true) : NewBoolData(false);
}

/* An IN node has left and list. */
Node *NewNodeIn(Node *left, NodeList *list) {
    Node *node = (Node *)calloc(1, sizeof(Node));
    node->type = NodeType_BinOp;
    node->data = NULL;
    node->left = left;
    node->right = NULL;
    node->list = list;
    node->evaluate = node_in_evaluate;
    return node;
}
