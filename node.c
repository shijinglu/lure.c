#include<assert.h>
#include <memory.h>
#include <stdlib.h>
#include "node.h"

/**
 * Node is resolvable if its value may be derived from context map
 * or function eval.
 */
bool node_resolvable(Node *node) {
    assert(node != NULL);
    switch (node->type) {
        case NodeType_BinOp:
        case NodeType_UnaryOp:
        case NodeType_Function:
        case NodeType_Identity:
            return true;
        default:
            break;
    }
    return false;
}

/* Recursively free all nodes and data contained. */
void free_node_deep(Node *node) {
    assert(node != NULL);
    if (node->left != NULL) {
        free_node_deep(node->left);
        node->left = NULL;
    }
    if (node->right != NULL) {
        free_node_deep(node->right);
        node->right = NULL;
    }
    if (node->data != NULL) {
        node->data->clean(node->data);
        free(node->data);
        node->data = NULL;
    }
    if (node->list != NULL) {
        node_list_free(node->list, true);
        node->list = NULL;
    }
    free(node);
}

Node *copy_node_deep(Node *node) {
    assert(node!=NULL);
    Node *dest = (Node *)calloc(1, sizeof(Node));
    memcpy(dest, node, sizeof(Node));
    if (node->left != NULL) {
        dest->left = copy_node_deep(node->left);
    }
    if (node->right != NULL) {
        dest->right = copy_node_deep(node->right);
    }
    if (node->data != NULL) {
        dest->data = node->data->copy(node->data);
    }
    if (node->list != NULL) {
        dest->list = node_list_new();
        for (int i=0; i<node->list->n_all; ++i) {
            Node *copy = copy_node_deep(node->list->all[i]);
            node_list_add(dest->list, copy);
        }
    }
    return dest;
}
