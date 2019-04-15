#ifndef __LURE_AST_BUILDER_H__
#define __LURE_AST_BUILDER_H__
#include "node.h"

/** Reducing functions */
NodeList *exprListOfExpr(Node *node);
NodeList *exprListAppend(NodeList *list, Node *node);

Node *exprUnaryOp(int uop, Node *node);
Node *exprBinOp(Node *lhs, int bop, Node *rhs);
Node *exprBetween(Node *xpLeft, Node *xpLower, Node *xpUpper);
Node *exprFunction0(char *fname);
Node *exprFunction(char *fname, NodeList *xpList);
Node *exprIn(Node *xpLeft, int opInOrNot, NodeList *xpList);
Node *exprOfInt(int n);
Node *exprOfDouble(double n);
Node *exprOfString(char *s);
Node *exprOfIdentity(char *s);

#endif  /*__LURE_AST_BUILDER_H__*/
