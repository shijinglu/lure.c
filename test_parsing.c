/**
 * This prog tests if strings can be parsed correctly.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lure.h"
#include "y.tab.h"

#define RED_FAILURE "\x1b[31m [FAILED] \x1b[0m"
#define GREEN_SUCCESS "\x1b[32m [SUCCESS] \x1b[0m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_RESET "\x1b[0m"

void expect(char *input, const char *exprKey, DataType leftType, DataType rightType, int nlist, int opcode) {
    Expr *xp = compile(input);
    int r1 = strncasecmp(xp->key, exprKey, strnlen(exprKey, MAX_IDENTIFIER_LENGTH));
    bool ckLeft = xp->pLeft == NULL ? leftType == DataUnknown : xp->pLeft->dataType == leftType;
    bool ckRight = xp->pRight == NULL ? rightType == DataUnknown : xp->pRight->dataType == rightType;
    bool ckList = xp->pList == NULL ? nlist == 0 : xp->pList->nExpr == nlist;
    bool ckOp = opcode == xp->op;
    if (r1 == 0 && ckLeft && ckRight && ckList && ckOp) {
        printf(GREEN_SUCCESS "%s is parsed correctly\n", input);
    }
    if (r1 != 0) fprintf(stderr, RED_FAILURE "expected key is: %s, while actual key is: %s\n", exprKey, xp->key);
    if (!ckLeft)
        fprintf(stderr, RED_FAILURE "expected left type is: %d, while actual type is: %d\n", leftType,
                xp->pLeft == NULL ? -1 : xp->pLeft->dataType);
    if (!ckRight)
        fprintf(stderr, RED_FAILURE "expected right type is: %d, while actual type is: %d\n", rightType,
                xp->pRight == NULL ? -1 : xp->pRight->dataType);
    if (!ckList)
        fprintf(stderr, RED_FAILURE "expected list length: %d, while actual length is: %d\n", nlist,
                xp->pList == NULL ? 0 : xp->pList->nExpr);
    if (!ckOp) fprintf(stderr, RED_FAILURE "expected op code is: %d, while actual code is: %d\n", opcode, xp->op);
}

void expectBetween(char *input, const char *leftKey, char *rightKey, EpType valueType) {
    Expr *xp = compile(input);
    int r1 = strncasecmp(xp->pLeft->key, leftKey, strnlen(leftKey, MAX_IDENTIFIER_LENGTH));
    int r2 = strncasecmp(xp->pRight->key, rightKey, strnlen(rightKey, MAX_IDENTIFIER_LENGTH));
    bool ckOp = xp->op == TK_AND && xp->pLeft->op == TK_GE && xp->pRight->op == TK_LE;
    bool ckType = xp->pLeft->type == EpBinOp && xp->pRight->type == EpBinOp && xp->pLeft->pLeft->type == EpLiteral &&
                  xp->pLeft->pRight->type == EpLiteral && xp->pRight->pLeft->type == EpLiteral &&
                  xp->pRight->pRight->type == EpLiteral;
    if (r1 == 0 && r2 == 0 && ckOp && ckType) {
        printf(GREEN_SUCCESS "[SUCCESS] %s is parsed correctly\n", input);
    }
    if (r1 != 0)
        fprintf(stderr, RED_FAILURE "[FAILED] expected key is: %s and %s, while actual key is: %s\n", leftKey, rightKey,
                xp->key);
    if (!ckOp)
        fprintf(stderr,
                RED_FAILURE
                "[FAILED] expected opcodes: %d, %d, %d, while actual ops are: "
                "%d, %d, %d\n",
                xp->op, TK_AND, TK_GE, TK_LE, xp->pLeft->op, xp->pRight->op);
    if (!ckType)
        fprintf(stderr,
                RED_FAILURE
                "[FAILED] expected types: %d, %d, %d, while actual ops are: "
                "%d, %d, %d\n",
                xp->type, EpBinOp, valueType, valueType, xp->pLeft->type, xp->pRight->type);
}

void test_in() {
    printf(ANSI_COLOR_YELLOW
           "\n################# Test In operator "
           "#################\n" ANSI_COLOR_RESET);
    expect("CITY_ID IN (1)", "CITY_ID in (1)", DataString, DataUnknown, 1, TK_IN);
    expect("CITY_ID IN (1)", "CITY_ID in (1)", DataString, DataUnknown, 1, TK_IN);
    expect("CITY_ID IN (1, 2, 3)", "CITY_ID in (1, 2, 3)", DataString, DataUnknown, 3, TK_IN);
    expect("USER_ID IN ('a', 'b', 'c')", "USER_ID in (a, b, c)", DataString, DataUnknown, 3, TK_IN);
    expect("VERSION IN ('v1.2.3', 'v2.3.4', 'v3.4.5')", "VERSION in (v1.2.3, v2.3.4, v3.4.5)", DataString, DataUnknown,
           3, TK_IN);
    expect("CITY_ID NOT IN (1)", "CITY_ID not in (1)", DataString, DataUnknown, 1, TK_NOTIN);
    expect("CITY_ID NOT IN (1, 2, 3)", "CITY_ID not in (1, 2, 3)", DataString, DataUnknown, 3, TK_NOTIN);
}

void test_binop() {
    printf(ANSI_COLOR_YELLOW
           "\n################# Test Bin-Op operator "
           "#################\n" ANSI_COLOR_RESET);
    expect("pValue > 0.5", "pValue > 0.5", DataString, DataDouble, 0, TK_GT);
    expect("pValue < 0.5", "pValue < 0.5", DataString, DataDouble, 0, TK_LT);
    expect("pValue >= 0.5", "pValue >= 0.5", DataString, DataDouble, 0, TK_GE);
    expect("pValue <= 0.5", "pValue <= 0.5", DataString, DataDouble, 0, TK_LE);
    expect("pValue == 0.5", "pValue == 0.5", DataString, DataDouble, 0, TK_EQ);
    expect("pValue != 0.5", "pValue != 0.5", DataString, DataDouble, 0, TK_NE);
    expect("VERSION > 'v1.2.3'", "VERSION > v1.2.3", DataString, DataString, 0, TK_GT);
    expect("currentTime >= 1424832200169", "currentTime >= 1424832200169", DataString, DataDouble, 0, TK_GE);
}

void test_between() {
    printf(ANSI_COLOR_YELLOW
           "\n################# Test Betwee operator "
           "#################\n" ANSI_COLOR_RESET);
    expectBetween("pValue Between [0.1, 0.5]", "pValue>=0.1", "pValue<=0.5", EpLiteral);
}

void test_function() {
    printf(ANSI_COLOR_YELLOW
           "\n################# Test Function "
           "#################\n" ANSI_COLOR_RESET);
    expect("md5mod(USER_UUID, 'abcdefg')", "md5mod(USER_UUID, abcdefg)", DataString, DataUnknown, 2, 0);
}

void test_andor() {
    printf(ANSI_COLOR_YELLOW
           "\n################# Test AND/OR operator "
           "#################\n" ANSI_COLOR_RESET);
    expect("condition1 and condition2", "condition1 and condition2", DataString, DataString, 0, TK_AND);
    expect("CITY_ID IN (1, 2, 3) && VERSION > 'v1.2.3'", "CITY_ID in (1, 2, 3) and VERSION > v1.2.3", DataUnknown,
           DataUnknown, 0, TK_AND);
    expect("(a == 1 && b != 2) && (x > 3 || y < 4 )", "a == 1 and b != 2 and x > 3 or y < 4", DataUnknown, DataUnknown, 0,
           TK_AND);
}

extern int yy_flex_debug;
extern int yydebug;

int main(int argc, char const *argv[]) {
    //    yy_flex_debug = 1;
    //    yydebug = 1;
    test_between();
    test_function();
    test_andor();
    test_in();
    test_binop();
    return 0;
}
