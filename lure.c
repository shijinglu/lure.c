#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "lure.h"
#include "node.h"
#include "hashmap.h"
#include "y.tab.h"

/* Create a new context object. */
ContextPtr NewContext(void) {
    return (ContextPtr) hashmap_new();
}

void setStringContext(ContextPtr ctx, char *key, char *val) {
    hashmap_put((map_t)ctx, key, NewStringData(val));
}

void setIntContext(ContextPtr ctx, char *key,  int val) {
    hashmap_put((map_t)ctx, key, NewIntData(val));
}

void setDoubleContext(ContextPtr ctx, char *key,  double val) {
    hashmap_put((map_t)ctx, key, NewDoubleData(val));
}

void setBoolContext(ContextPtr ctx, char *key,  bool val) {
    hashmap_put((map_t)ctx, key, NewBoolData(val));
}

void setCustomContext(ContextPtr ctx, char *key,  char *val, char *typeDesc) {
    if (strcmp(typeDesc, "semver") == 0) {
        hashmap_put((map_t)ctx, key, NewVersionData(val));
        return;
    }
    printf("unsupported data type: %s", typeDesc);
    exit(-1);
}

int print_context_item(void *item, void *data) {
    LURE_ASSERT(data != NULL, "data should not be null");
    Data *val = (Data *)data;
    int *idx = (int *)item;
    printf("Context[%d]: <%s> \n", *idx, val->getCStr(val));
    *idx = *idx + 1;
    return MAP_OK;
}

void printContext(ContextPtr ctx) {
    int i = 0;
    hashmap_iterate((map_t)ctx, print_context_item, &i);
}

typedef struct yy_buffer_state *YY_BUFFER_STATE;
extern int yyparse(NodeList **rootExprList);
extern YY_BUFFER_STATE yy_scan_string(char *str);
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);

Node *lure_compile(const char *s) {
    YY_BUFFER_STATE buffer = yy_scan_string((char *)s);
    NodeList *rootList = NULL;
    yyparse(&rootList);
    yy_delete_buffer(buffer);
    Node *xp = rootList->all[0];
    node_list_free(rootList, false);
    return xp;
}

bool lure_compile_eval(Node *node, ContextPtr ctx) {
    Data *data = node->evaluate(node, ctx);
    bool flag = data->toBoolean(data);
    data->clean(data); free(data);
    return flag;
}

bool lure_eval(ContextPtr ctx, const char *s) {
    Node *node = lure_compile(s);
    return lure_compile_eval(node, ctx);
}
