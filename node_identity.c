#include<assert.h>
#include<stdlib.h>
#include<memory.h>
#include "node.h"
#include "data.h"
#include "util.h"
#include "hashmap.h"

/* Look up context and return resolved data if found. Otherwise, return
 copy of internal data. */
Data *node_identity_evaluate(Node *node, map_t context) {
    assert(node != NULL);
    if (node->data == NULL) {
        return NewBoolData(false);
    }
    if (context == NULL) {
        return node->data->copy(node->data);
    }
    char* key = node->data->getCStr(node->data);
    Data *element = NULL;
    int code = hashmap_get(context, key, (void **)(&element));
    if (code == MAP_MISSING || element == NULL) {
        return node->data->copy(node->data);
    } else {
        return element->copy(element);
    }
}

Node *NewNodeIdentity(const char *s) {
    Node *node = (Node *)calloc(1, sizeof(Node));
    node->type = NodeType_Identity;
    node->data = NewStringData(s);
    node->left = NULL;
    node->right = NULL;
    node->list = NULL;
    node->evaluate = node_identity_evaluate;
    return node;
}
