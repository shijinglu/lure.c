#include<assert.h>
#include<stdlib.h>
#include "node.h"
#include "data.h"
#include "util.h"


/* evaluate, allocate and return a data struct. */
Data *node_binop_evaluate(Node *node, map_t context) {
    LURE_ASSERT(node != NULL, "cannot evaluate against NULL node");
    LURE_ASSERT(node->left != NULL, "left side of a binary operation must not be empty");
    LURE_ASSERT(node->right != NULL, "right side of a binary operation must not be empty");
    Data *leftRet = node->left->evaluate(node->left, context);
    Data *rightRet = node->right->evaluate(node->right, context);
    bool flag = false;
    if (node->op == BinOpType_OR) {
        flag = leftRet->toBoolean(leftRet) || rightRet->toBoolean(rightRet);
        goto eval_return;
    }else if(node->op == BinOpType_AND) {
        flag = leftRet->toBoolean(leftRet) && rightRet->toBoolean(rightRet);
        goto eval_return;
    }
    int cmp = leftRet->compareTo(leftRet, rightRet);
    switch (node->op) {
        case BinOpType_EQ:
            flag = cmp == 0;
            break;
         case BinOpType_GE:
            flag = cmp >= 0;
            break;
        case BinOpType_LE:
            flag = cmp <= 0;
            break;
        case BinOpType_GT:
            flag = cmp > 0;
            break;
        case BinOpType_LT:
            flag = cmp < 0;
            break;
        case BinOpType_NE:
            flag = cmp != 0;
            break;
        default:
            break;
    }
eval_return:
    leftRet->clean(leftRet); free(leftRet);
    rightRet->clean(rightRet); free(rightRet);
    return NewBoolData(flag);
}

Node *NewNodeBinOp(BinOpType op, Node *left, Node *right) {
    LURE_ASSERT(left != NULL, "left side of a binary operation must not be empty");
    LURE_ASSERT(right != NULL, "right side of a binary operation must not be empty");
    Node *node = (Node *)calloc(1, sizeof(Node));
    node->op = op;
    node->type = NodeType_BinOp;
    node->left = left;
    node->right = right;
    node->list = NULL;
    node->evaluate = node_binop_evaluate;
    return node;
}
