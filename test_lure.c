
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "node.h"


map_t example_context() {
    static map_t m = NULL;
    if (m == NULL) {
        m = hashmap_new();
        hashmap_put(m, "USER_ID", NewStringData("alice"));
        hashmap_put(m, "CITY_ID", NewIntData(123));
        hashmap_put(m, "PI", NewDoubleData(3.14));
        hashmap_put(m, "MESSAGE", NewStringData("hello world"));
        hashmap_put(m, "v3.2.1", NewVersionData("v3.2.1"));
    }
    return m;
}

NodeList *new_example_node_list(int nitems) {
    NodeList *list = node_list_new();
    node_list_add(list, NewStringLiteral("hello world"));
    node_list_add(list, NewIntLiteral(100));
    if (nitems == 2) {
        return list;
    }
    node_list_add(list, NewDoubleLiteral(3.14));
    node_list_add(list, NewNodeIdentity("USER_ID"));
    return list;
}

void test_node_literal() {
    Data *strData = NewStringData("USER_ID");
    Node *node = NewNodeLiteral(strData);
    Data *ret = node->evaluate(node, example_context());
    assert(strcmp(ret->getCStr(ret), "USER_ID") == 0);
    ret->clean(ret); free(ret);
    free_node_deep(node);
    strData->clean(strData); free(strData);
    printf("[ok] test_node_literal \n");
}

void test_node_identity() {
    Node *node = NewNodeIdentity("USER_ID");
    Data *ret = node->evaluate(node, example_context());
    assert(strcmp(ret->getCStr(ret), "alice") == 0);
    ret->clean(ret); free(ret);
    free_node_deep(node);
    printf("[ok] test_node_identity \n");
}

void test_node_binop() {
    Node *node = NewNodeBinOp(BinOpType_EQ, NewNodeIdentity("USER_ID"), NewStringLiteral("alice"));
    Data *ret = node->evaluate(node, example_context());
    assert(ret->toBoolean(ret) == true);
    ret->clean(ret); free(ret);
    free_node_deep(node);
    
    node = NewNodeBinOp(BinOpType_GE, NewNodeIdentity("USER_ID"), NewStringLiteral("bob"));
    ret = node->evaluate(node, example_context());
    assert(ret->toBoolean(ret) == false);
    ret->clean(ret); free(ret);
    free_node_deep(node);
    printf("[ok] test_node_binop \n");
}

void test_node_function() {
    Node *node = NewNodeFunction("md5mod", new_example_node_list(2));
    Data *ret = node->evaluate(node, example_context());
    assert(ret->toInt(ret) == 28);
    ret->clean(ret); free(ret);
    free_node_deep(node);
    printf("[ok] test_node_function \n");
}

void test_node_in() {
    Node *node = NewNodeIn(NewNodeIdentity("PI"), new_example_node_list(4));
    Data *ret = node->evaluate(node, example_context());
    assert(ret->toBoolean(ret) == true);
    ret->clean(ret); free(ret);
    free_node_deep(node);
    
    node = NewNodeIn(NewNodeIdentity("MESSAGE"), new_example_node_list(4));
    ret = node->evaluate(node, example_context());
    assert(ret->toBoolean(ret) == true);
    ret->clean(ret); free(ret);
    free_node_deep(node);
    
    printf("[ok] test_node_in \n");
}

void test_node_like() {
    Node *node = NewNodeLike(NewStringLiteral("hello*"), NewStringLiteral("hello world"));
    Data *ret = node->evaluate(node, example_context());
    assert(ret->toBoolean(ret) == true);
    ret->clean(ret); free(ret);
    free_node_deep(node);
    printf("[ok] test_node_like \n");
}

void test_mem_leak() {
    for (int i=0; i<15000; i++) {
        usleep(1000);
        test_node_literal();
        test_node_identity();
        test_node_binop();
        test_node_function();
        test_node_in();
        test_node_like();
    }
}

int main(int argc, const char * argv[]) {
    test_node_literal();
    test_node_identity();
    test_node_binop();
    test_node_function();
    test_node_in();
    test_node_like();
    //    test_mem_leak();
    return 0;
}
