#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "ast_builder.h"
#include "node.h"
#include "y.tab.h"

NodeList *exprListOfExpr(Node *node) {
    LURE_LOG("exprListOfExpr");
    NodeList *list = node_list_new();
    node_list_add(list, node);
    return list;
}

NodeList *exprListAppend(NodeList *list, Node *node) {
    node_list_add(list, node);
    return list;
}

/** Currently only support `NOT xxx` */
Node *exprUnaryOp(int uop, Node *node) {
    if (uop == TK_NOT) {
        return NewNodeBinOp(BinOpType_EQ, node, NewBooleanLiteral(false));
    }
    LURE_ERROR("Unsupported unary operator");
    return NULL;
}

BinOpType map_token_to_binop(int bop) {
    switch (bop) {
        case TK_EQ:
            return BinOpType_EQ;
        case TK_NE:
            return BinOpType_NE;
        case TK_GE:
            return BinOpType_GE;
        case TK_GT:
            return BinOpType_GT;
        case TK_LT:
            return BinOpType_LT;
        case TK_LE:
            return BinOpType_LE;
        case TK_AND_LOGIC:
            return BinOpType_AND;
        case TK_OR_LOGIC:
            return BinOpType_OR;
        default:
            LURE_ERROR("Unsupported binary operator");
            break;
    }
    return BinOpType_EQ;
}

Node *exprBinOp(Node *lhs, int bop, Node *rhs) {
    return NewNodeBinOp(map_token_to_binop(bop), lhs, rhs);
}

Node *exprBetween(Node *xpLeft, Node *xpLower, Node *xpUpper) {
    /*FIXME: deallocate xpLeft may run into problems. */
    Node *leftCopy = copy_node_deep(xpLeft);
    return NewNodeBinOp(BinOpType_AND,
                        NewNodeBinOp(BinOpType_GE, xpLeft, xpLower),
                        NewNodeBinOp(BinOpType_LE, leftCopy, xpUpper));
}


Node *exprFunction0(char *fname) {
    NodeList *list = node_list_new();
    return NewNodeFunction(fname, list);
}

Node *exprFunction(char *fname, NodeList *xpList) {
    return NewNodeFunction(fname, xpList);
}

Node *exprIn(Node *xpLeft, int opInOrNot, NodeList *xpList) {
    Node * node = NewNodeIn(xpLeft, xpList);
    if (opInOrNot == TK_IN) {
        return node;
    } else if (opInOrNot == TK_NOTIN) {
        return exprUnaryOp(TK_NOT, node);
    }
     LURE_ERROR("Unexpected operator in an 'in' expression. ");
    return NULL;
}

Node *exprOfInt(int n) {
    return NewIntLiteral(n);
}

Node *exprOfDouble(double n) {
    return NewDoubleLiteral(n);
}

Node *exprOfString(char *s) {
    return NewStringLiteral(s);
}

Node *exprOfIdentity(char *s) {
    return NewNodeIdentity(s);
}
