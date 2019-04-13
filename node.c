#include<assert.h>
#include <stdlib.h>
#include "node.h"

/**
 * Node is resolvable if its value may be derived from context map
 * or function eval.
 */
bool node_resolvable(Node *node) {
    assert(node != NULL);
    return (node->type == NodeType_Function || node->type == NodeType_Identity);
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
        node_list_free(node->list);
        node->list = NULL;
    }
    free(node);
}
