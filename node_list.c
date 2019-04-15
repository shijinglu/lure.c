#include<stdlib.h>
#include<string.h>
#include<assert.h>
#include "node.h"
#include "data.h"
#include "hashmap.h"

void node_list_add(NodeList *self, Node *node) {
    assert(self != NULL);
    assert(node != NULL);
    if (self->n_all + 2 >= self->capacity) {
        int capacity = self->capacity * 2;
        self->all = (Node**)realloc(self->all, sizeof(Node*) * capacity);
        self->unresolved = (Node**)realloc(self->unresolved, sizeof(Node*) * capacity);
        self->capacity = capacity;
    }
    self->all[self->n_all++] = node;
    if (node_resolvable(node)) {
        self->resolvable = true;
        self->unresolved[self->n_unresolved++] = node;
    } else {
        Data *val = node->data;
        char *key = val->getCStr(val);
        hashmap_put(self->resolved, key, val);
    }

}

bool node_list_is_resolvable(NodeList *self) {
    return self->resolvable;
}


bool node_list_in(NodeList *self, Data *data, map_t context) {
    Data *resolved = NULL;
    Node *node = NULL;
    bool flag = false;
    if (data->rawType == RawDataString) {
        Data *out = NULL;
        int ok = hashmap_get(self->resolved, data->getCStr(data), (void**)(&out));
        if (ok == MAP_OK) {
            return true;
        }
        for (int i = 0; i < self->n_unresolved; ++i) {
            node = self->unresolved[i];
            resolved = node->evaluate(node, context);
            if (resolved->compareTo(resolved, data) == 0) {
                flag = true;
                goto node_list_in_ret;
            }
            resolved->clean(resolved);
            free(resolved);
            resolved = NULL;
        }
        return false;
    } else {
        for (int i = 0; i < self->n_all; ++i) {
            node = self->all[i];
            if (node_resolvable(node)) {
                resolved = node->evaluate(node, context);
                if (resolved->compareTo(resolved, data) == 0) {
                    flag = true;
                    goto node_list_in_ret;
                }
                resolved->clean(resolved);
                free(resolved);
                resolved = NULL;
            } else if (node->data->compareTo(node->data, data) == 0) {
                return true;
            }
        }
    }
node_list_in_ret:
    if (resolved != NULL) {
        resolved->clean(resolved); free(resolved); resolved = NULL;
    }
    return flag;
}

NodeList *node_list_new() {
    NodeList *list = (NodeList *)calloc(1, sizeof(NodeList));
    int capacity = NODE_LIST_DEFAULT_CAPACITY;
    list->capacity = capacity;
    list->all = (Node **)calloc(capacity, sizeof(Node*));
    list->n_all = 0;
    list->unresolved = (Node **)calloc(capacity, sizeof(Node*));
    list->n_unresolved = 0;
    list->resolved = hashmap_new();
    return list;
}

void node_list_free(NodeList *list, bool deep) {
    if (deep) {
        for (int i=0; i<list->n_all; ++i) {
            free_node_deep(list->all[i]);
        }
    }
    free(list->all); list->all = NULL;
    free(list->unresolved); list->unresolved = NULL;
    hashmap_free(list->resolved); list->resolved = NULL;
    free(list);
}
