#include<assert.h>
#include<stdlib.h>
#include<memory.h>
#include "node.h"
#include "data.h"
#include "util.h"
#include "hashmap.h"
#include "re.h"

/* evaluate, allocate and return a data struct. */
Data *node_literal_evaluate(Node *node, map_t context) {
    LURE_ASSERT(node != NULL, "cannot evaluate against an empty node");
    if (node->data == NULL) {
        return NewBoolData(false);
    }
    return node->data->copy(node->data);
}

/* create a node without creating new data object. */
Node *new_node_literal_nocopy(Data *data) {
    Node *node = (Node *)calloc(1, sizeof(Node));
    node->type = NodeType_Literal;
    node->data = data;
    node->left = NULL;
    node->right = NULL;
    node->list = NULL;
    node->evaluate = node_literal_evaluate;
    return node;
}
/*Make a copy of data object*/
Node *NewNodeLiteral(Data *data) {
    return new_node_literal_nocopy(data->copy(data));
}

Node *NewBooleanLiteral(bool val) {
    return new_node_literal_nocopy(NewBoolData(val));
}

Node *NewIntLiteral(int val) {
    return new_node_literal_nocopy(NewIntData(val));
}

Node *NewDoubleLiteral(double val) {
    return new_node_literal_nocopy(NewDoubleData(val));
}

Node *NewStringLiteral(const char * val) {
    return new_node_literal_nocopy(NewStringData(val));
}

