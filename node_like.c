#include<assert.h>
#include<stdlib.h>
#include<memory.h>
#include "node.h"
#include "data.h"
#include "util.h"
#include "hashmap.h"
#include "re.h"

/* evaluate, allocate and return a data struct. */
Data *node_like_evaluate(Node *node, map_t context) {
    LURE_ASSERT(node != NULL, "cannot evaluate against an empty node");
    LURE_ASSERT(node->left != NULL, "left side of a <like> node must not be NULL");
    LURE_ASSERT(node->right != NULL, "right side of a <like> node must not be NULL");

    Data *leftRes = node->left->evaluate(node->left, context);
    Data *rightRes = node->right->evaluate(node->right, context);
    int m = re_match(leftRes->getCStr(leftRes), rightRes->getCStr(rightRes));
    leftRes->clean(leftRes); free(leftRes);
    rightRes->clean(rightRes); free(rightRes);
    return NewBoolData(m != -1);
}

Node *NewNodeLike(Node *left, Node *right) {
    Node *node = (Node *)calloc(1, sizeof(Node));
    node->type = NodeType_Identity;
    node->left = left;
    node->right = right;
    node->list = NULL;
    node->evaluate = node_like_evaluate;
    return node;
}
