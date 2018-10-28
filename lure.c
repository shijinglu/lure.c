#include "lure.h"
#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lure_extension.h"
#include "md5hash.h"
#include "y.tab.h"

#define MAX_NUM_LITERAL_LENGTH 128

// #define LURE_LOG(msg, args...) printf("[LURE LOG]: %s(%s:%d) %s \n",
// __func__,__FILE__, __LINE__, msg, ##args)
#define LURE_LOG(msg, args...) \
    do {                       \
    } while (0)

Expr *newExpr() {
    LURE_LOG("newExpr");
    Expr *xp = (Expr *)malloc(sizeof(Expr));
    if (!xp) {
        LURE_LOG("failed to allocate expression node");
        exit(1);
    }
    xp->key = NULL;
    xp->data.stringVal = NULL;
    xp->pLeft = NULL;
    xp->pRight = NULL;
    xp->pList = NULL;
    xp->ext = NULL;
    xp->extKey = NULL;
    xp->derivedValue = NULL;
    return xp;
}

void freeExpr(Expr *xp) {
    LURE_LOG("freeExpr");
    if (!xp) return;
    free(xp->key);
    xp->key = NULL;
    free(xp->data.stringVal);
    xp->data.stringVal = NULL;
    if (xp->pLeft) {
        freeExpr(xp->pLeft);
        xp->pLeft = NULL;
    }
    if (xp->pRight) {
        freeExpr(xp->pRight);
        xp->pRight = NULL;
    }
    if (xp->pList) {
        freeExprList(xp->pList);
        xp->pList = NULL;
    }
    free(xp->ext);
    free(xp->extKey);
    free(xp->derivedValue);
    free(xp);
    xp = NULL;
}

ExprList *newExprList() {
    LURE_LOG("newExprList");
    ExprList *xpList = (ExprList *)malloc(sizeof(ExprList));
    if (!xpList) {
        LURE_LOG("failed to allocate expression list");
        exit(1);
    }
    xpList->nExpr = DEFAULT_EXPR_LIST_LENGTH;
    xpList->key = NULL;
    xpList->list = (Expr **)malloc(sizeof(Expr *) * DEFAULT_EXPR_LIST_LENGTH);
    xpList->map = hashmap_new();
    xpList->capacity = DEFAULT_EXPR_LIST_LENGTH;
    for (size_t i = 0; i < xpList->capacity; i++) {
        xpList->list[i] = NULL;
    }
    return xpList;
}

void freeExprList(ExprList *xpList) {
    LURE_LOG("freeExprList");
    if (!xpList) return;
    for (size_t i = 0; i < xpList->nExpr; i++) {
        freeExpr(xpList->list[i]);
    }
    free(xpList->list);
    xpList->list = NULL;
    xpList->capacity = 0;
    hashmap_free(xpList->map);
    xpList->map = NULL;
    free(xpList->key);
    xpList->key = NULL;
    free(xpList);
    xpList = NULL;
}

Context *newContext() {
    LURE_LOG("newContext");
    Context *ctx = (Context *)malloc(sizeof(Context));
    if (!ctx) {
        LURE_LOG("failed to allocate expression list");
        return NULL;
    }
    ctx->customDataType = NULL;
    ctx->customData = NULL;
    ctx->data.boolVal = false;
    ctx->data.intVal = 0;
    ctx->data.doubleVal = 0;
    ctx->data.stringVal = NULL;
    ctx->dataType = DataUnknown;
    return ctx;
}

void freeContext(Context *ctx, void (*customDealloc)(Context *)) {
    LURE_LOG("freeContext");
    if (ctx == NULL) {
        return;
    }
    free(ctx->customDataType);
    ctx->customDataType = NULL;
    if (customDealloc != NULL) {
        customDealloc(ctx);
    }
    free(ctx->data.stringVal);
    ctx->data.stringVal = NULL;
    free(ctx);
    ctx = NULL;
}

/**
 * Create a new string by concatenate all strings,
 * eg:`char *str = strconcat(3, "testing ", "this ", "thing");`
 * NOTE: returned string needs to be freed later.
 * */
char *strconcat(int num_args, ...) {
    LURE_LOG("strconcat [0]");
    int strsize = 0;
    va_list ap;
    va_start(ap, num_args);
    for (int i = 0; i < num_args; i++) {
        strsize += strlen(va_arg(ap, char *));
    }

    char *res = malloc(strsize + 1);
    strsize = 0;
    va_start(ap, num_args);
    for (int i = 0; i < num_args; i++) {
        char *s = va_arg(ap, char *);
        strcpy(res + strsize, s);
        strsize += strlen(s);
    }
    va_end(ap);
    res[strsize] = '\0';
    LURE_LOG("strconcat [1]");
    return res;
}

char *newStrOfInt(int opcode) {
    LURE_LOG("newStrOfInt");
    char numStr[MAX_NUM_LITERAL_LENGTH];
    int cx = snprintf(numStr, MAX_NUM_LITERAL_LENGTH, "%d", opcode);
    if (cx >= 0 && cx < MAX_NUM_LITERAL_LENGTH) {
        numStr[MAX_NUM_LITERAL_LENGTH - 1] = '\0';
        char *res = (char *)malloc(sizeof(char) * (cx + 1));
        strcpy(res, numStr);
        return res;
    }
    return NULL;
}

/* Test if all characters in s are letters, digits or underlines. */
bool isValidIdentifier(char *s) {
    int i = 0;
    for (; s[i] && i <= MAX_IDENTIFIER_LENGTH; i++) {
        char c = s[i];
        if (!isalnum(c) && c != '.' && c != '_') {
            return false;
        }
    }
    return i <= MAX_IDENTIFIER_LENGTH;
}

char *newStrOfDouble(double num) {
    LURE_LOG("newStrOfDouble");
    char numStr[MAX_NUM_LITERAL_LENGTH];
    int cx = snprintf(numStr, MAX_NUM_LITERAL_LENGTH, "%f", num);
    if (cx >= 0 && cx < MAX_NUM_LITERAL_LENGTH) {
        numStr[MAX_NUM_LITERAL_LENGTH - 1] = '\0';
        char *res = (char *)malloc(sizeof(char) * (cx + 1));
        strcpy(res, numStr);
        return res;
    }
    return NULL;
}

/** Parse semetic version from string like "v1.2.3"
 * v1.2.3 => 1.2.3 [true]
 * v1..2..3 => 1.2.3 [true]
 * v1.2 => 1.2.0 [true]
 * v1 => 1.0.0 [true]
 * v0.1.alpha => 0.1.0 [true]
 */
bool parseSemVer(char *s, int (*ver)[3]) {
    if (s == NULL || ver == NULL) {
        return false;
    }
    (*ver)[0] = 0;
    (*ver)[1] = 0;
    (*ver)[2] = 0;
    size_t m = strnlen(s, MAX_SEMVER_STR_LENGTH);
    int vi = 0;
    for (int i = 0, j = 0; i < m && j <= m && vi < 3; j++) {
        if (s[i] == 'v' || s[i] == 'V') {
            j = i++;
        } else if (!isdigit(s[j])) {
            if (j > i) (*ver)[vi++] = atoi(&s[i]);
            i = j + 1;
        }
    }
    return vi > 0;
}

char *descOfOp(int opcode) {
    LURE_LOG("descOfOp");
    switch (opcode) {
        case TK_AND:
            return "and";
        case TK_EQ:
            return "==";
        case TK_GE:
            return ">=";
        case TK_GT:
            return ">";
        case TK_IN:
            return "in";
        case TK_LE:
            return "<=";
        case TK_LT:
            return "<";
        case TK_MD5MOD:
            return "md5mod";
        case TK_NE:
            return "!=";
        case TK_NOT:
            return "not";
        case TK_NOTIN:
            return "not in";
        case TK_INT_LITERAL:
            return "an integer";
        case TK_DOUBLE_LITERAL:
            return "a dobule";
        case TK_OR:
            return "or";
        case TK_STRCMP:
            return "strcmp";
        case TK_STRING_LITERAL:
            return "a string";
        default:
            return "undefined operator";
    }
}

void logExpr(Expr *xp) {
    if (!xp) {
        LURE_LOG("[Expr] null\n");
    } else {
        LURE_LOG("[Expr] %s\n", xp->key);
    }
}

ExprList *exprListOfExpr(Expr *xp) {
    LURE_LOG("exprListOfExpr");
    if (xp->key == NULL || strlen(xp->key) == 0) {
        LURE_LOG("xp->key is empty, cannot create expr list from an empty key");
        return NULL;
    }
    ExprList *xpList = newExprList();
    xpList->key = strconcat(2, "", xp->key);
    xpList->list[0] = xp;
    hashmap_put(xpList->map, xp->key, xp);
    xpList->nExpr = 1;
    return xpList;
}

ExprList *exprListAppend(ExprList *xpList, Expr *xp) {
    LURE_LOG("exprListAppend");
    if (xpList == NULL || xp == NULL || strlen(xp->key) == 0) {
        LURE_LOG("cannot append empty to empty");
        return xpList;
    }
    char *tmp = xpList->key;
    xpList->key = strconcat(3, tmp, ", ", xp->key);
    free(tmp);
    hashmap_put(xpList->map, xp->key, xp);
    if (xpList->nExpr >= xpList->capacity) {
        xpList->capacity *= 2;
        xpList->list = (Expr **)realloc(xpList->list, sizeof(Expr *) * xpList->capacity);
    }
    xpList->list[xpList->nExpr++] = xp;
    /*LOG*/
    LURE_LOG("[ExprList] list \n");
    return xpList;
}

Expr *exprLiteral(Expr *xpLiteral) {
    LURE_LOG("");
    return xpLiteral;
}

Expr *exprUnaryOp(int uop, Expr *xp) {
    LURE_LOG("");
    Expr *xp2 = newExpr();
    xp2->op = uop;
    xp2->pRight = xp;
    xp2->key = strconcat(2, descOfOp(uop), xp->key);
    xp2->type = EpUnaryOp;
    logExpr(xp);
    return xp2;
}

Expr *exprBinOp(Expr *xp1, int bop, Expr *xp2) {
    LURE_LOG("");
    Expr *xp = newExpr();
    xp->op = bop;
    xp->pLeft = xp1;
    xp->pRight = xp2;
    xp->key = strconcat(5, xp1->key, " ", descOfOp(bop), " ", xp2->key);
    xp->type = EpBinOp;
    logExpr(xp);
    return xp;
}

/* x BETWEEN (a, b) => (x>=a) AND (x<=b)*/
Expr *exprBetween(Expr *xpLeft, Expr *xpLower, Expr *xpUpper) {
    LURE_LOG("");
    Expr *xpA = newExpr();
    xpA->op = TK_GE;
    xpA->pLeft = xpLeft;
    xpA->pRight = xpLower;
    xpA->key = strconcat(3, xpA->pLeft->key, descOfOp(TK_GE), xpA->pRight->key);
    xpA->type = EpBinOp;

    Expr *xpB = newExpr();
    xpB->op = TK_LE;
    xpB->pLeft = xpLeft;
    xpB->pRight = xpUpper;
    xpB->key = strconcat(3, xpB->pLeft->key, descOfOp(TK_LE), xpB->pRight->key);
    xpB->type = EpBinOp;

    Expr *xp = newExpr();
    xp->op = TK_AND;
    xp->pLeft = xpA;
    xp->pRight = xpB;
    xp->key = strconcat(5, xpA->key, " ", descOfOp(TK_AND), " ", xpB->key);
    xp->type = EpBinOp;
    logExpr(xp);
    return xp;
}

Expr *exprFunction(char *fname, ExprList *xpList) {
    LURE_LOG("");
    Expr *xp = newExpr();
    /*TODO: replace me with TK_FUNCTION*/
    xp->pLeft = newExpr();
    xp->pLeft = exprOfString(fname);
    xp->pList = xpList;
    xp->key = strconcat(4, fname, "(", xpList->key, ")");
    xp->type = EpFunction;
    logExpr(xp);
    return xp;
}

Expr *exprIn(Expr *xpLeft, int opInOrNot, ExprList *xpList) {
    LURE_LOG("");
    Expr *xp = newExpr();
    xp->op = opInOrNot;
    xp->pLeft = xpLeft;
    xp->pList = xpList;
    xp->key = strconcat(6, xpLeft->key, " ", descOfOp(opInOrNot), " (", xpList->key, ")");
    xp->type = EpBinOp;
    logExpr(xp);
    return xp;
}

Expr *exprOfInt(int n) {
    LURE_LOG("");
    Expr *xp = newExpr();
    xp->key = newStrOfInt(n);
    xp->type = EpLiteral;
    xp->dataType = DataInt;
    xp->data.intVal = n;
    logExpr(xp);
    return xp;
}

Expr *exprOfDouble(double n) {
    LURE_LOG("");
    Expr *xp = newExpr();
    xp->key = newStrOfDouble(n);
    xp->type = EpLiteral;
    xp->dataType = DataDouble;
    xp->data.doubleVal = n;
    logExpr(xp);
    return xp;
}

Expr *exprOfString(char *s) {
    LURE_LOG("");
    Expr *xp = newExpr();
    xp->key = strconcat(1, s);
    xp->type = EpLiteral;
    xp->dataType = DataString;
    xp->data.stringVal = strconcat(1, s);
    logExpr(xp);
    return xp;
}

/* value getters */
DataType getDataType(Expr *expr) {
    if (expr == NULL) {
        return DataUnknown;
    }
    return (expr->derivedValue == NULL || expr->derivedDataType == DataUnknown) ? expr->dataType
                                                                                : expr->derivedDataType;
}

char *getStringData(Expr *expr) {
    if (expr == NULL) {
        return NULL;
    }
    return (expr->derivedValue == NULL || expr->derivedValue->stringVal == NULL) ? expr->data.stringVal
                                                                                 : expr->derivedValue->stringVal;
}

bool getBooleanData(Expr *expr) {
    if (expr == NULL) {
        return false;
    }
    return expr->derivedValue == NULL ? expr->data.boolVal : expr->derivedValue->boolVal;
}

int getIntData(Expr *expr) {
    if (expr == NULL) {
        return 0;
    }
    return expr->derivedValue == NULL ? expr->data.intVal : expr->derivedValue->intVal;
}
double getDoubleData(Expr *expr) {
    if (expr == NULL) {
        return 0;
    }
    return expr->derivedValue == NULL ? expr->data.doubleVal : expr->derivedValue->doubleVal;
}
void *getGenericData(Expr *expr) {
    if (expr == NULL) {
        return NULL;
    }
    return expr->derivedValue == NULL ? expr->data.genericVal : expr->derivedValue->genericVal;
}

/* evaluate expression with ctx, return false in case of error. */
bool eval(map_t ctx, Expr *root) {
    LURE_LOG("");
    int errCode = 0;
    bool res = evalInternal(ctx, root, &errCode, false);
    return errCode == 0 ? res : false;
}

typedef struct MD5Param {
    int amp;
    unsigned char *uuid;
    int uuidLen;
} MD5Param;

void freeMD5Param(MD5Param *ptr) {
    if (!ptr) return;
    if (ptr->uuid) {
        free(ptr->uuid);
    }
    free(ptr);
}

// int (*PFany)(any_t, any_t)
int md5modVisitor(any_t item, any_t data) {
    if (item == NULL || data == NULL) {
        return MAP_MISSING;
    }
    MD5Param *pm = (MD5Param *)item;
    Expr *xp = (Expr *)data;
    if (xp->data.stringVal) {
        size_t sn = strnlen(xp->data.stringVal, MAX_IDENTIFIER_LENGTH);
        pm->uuidLen = (int)sn;
        pm->uuid = (unsigned char *)malloc(sizeof(unsigned char) * (sn + 1));
        memcpy(pm->uuid, xp->data.stringVal, sizeof(unsigned char) * (sn + 1));
    } else if (xp->data.intVal) {
        pm->amp = xp->data.intVal;
    }
    return MAP_OK;
}

int semVerCmp(int (*left)[3], int (*right)[3]) {
    if (left == NULL && right == NULL) {
        return 0;
    }
    if (left == NULL) {
        return -1;
    }
    if (right == NULL) {
        return 1;
    }
    for (int i = 0; i < 3; i++) {
        if ((*left)[i] != (*right)[i]) {
            return (*left)[i] - (*right)[i];
        }
    }
    return 0;
}

bool hashmap_contains(map_t map, char *key) {
    void *elem = NULL;
    int code = hashmap_get(map, key, &elem);
    return code == MAP_OK && elem != NULL;
}

bool boolOfString(char *s) {
    if (s == NULL || s[0] == '\0' || strcasecmp(s, "false") == 0) {
        return false;
    }
    return true;
}

/* Cast data of xpRight to boolean. */
bool typeCastBool(Expr *xpRight, int *err) {
    if (!xpRight) {
        return false;
    }
    switch (getDataType(xpRight)) {
        case DataBool:
            return getBooleanData(xpRight);
        case DataInt:
            return (bool)getIntData(xpRight);
        case DataDouble:
            return (bool)getDoubleData(xpRight);
            return true;
        case DataString:
        case DataVersion:
            return boolOfString(getStringData(xpRight));
        default:
            break;
    }
    *err = Err_BadInput;
    return false;
}

int typeCastInt(Expr *xpRight, int *err) {
    if (!xpRight) {
        return 0;
    }
    switch (getDataType(xpRight)) {
        case DataBool:
            return (int)getBooleanData(xpRight);
        case DataInt:
            return getIntData(xpRight);
        case DataDouble:
            return (int)getDoubleData(xpRight);
        case DataString: {
            char *s = getStringData(xpRight);
            if (s == NULL || s[0] == '\0') {
                return 0;
            }
            return atoi(s);
        }
        default:
            break;
    }
    *err = Err_BadInput;
    return 0;
}

double typeCastDouble(Expr *xpRight, int *err) {
    if (!xpRight) {
        return 0.0;
    }
    switch (getDataType(xpRight)) {
        case DataBool:
            return getBooleanData(xpRight) ? 1.0 : 0.0;
        case DataInt:
            return (double)getIntData(xpRight);
        case DataDouble:
            return getDoubleData(xpRight);
        case DataString: {
            char *s = getStringData(xpRight);
            if (s == NULL || s[0] == '\0') {
                return 0.0;
            }
            return atof(s);
        }
        default:
            break;
    }
    *err = Err_BadInput;
    return 0.0;
}

char *newStringFromCast(Expr *xpRight, int *err) {
    if (!xpRight) {
        return NULL;
    }
    switch (getDataType(xpRight)) {
        case DataBool:
            return strconcat(1, getBooleanData(xpRight) ? "true" : "false");
        case DataInt:
            return newStrOfInt(getIntData(xpRight));
        case DataDouble:
            return newStrOfDouble(getDoubleData(xpRight));
        case DataString:
            return strconcat(1, getStringData(xpRight));
        default:
            break;
    }
    *err = Err_BadInput;
    return NULL;
}

bool typeCastVersion(Expr *xpRight, int (*ver)[3]) {
    switch (getDataType(xpRight)) {
        case DataBool:
            return false;
        case DataInt:
            (*ver)[0] = xpRight->data.intVal;
            return true;
        case DataDouble: {
            /* add 1e-9 to avoid cases like 0.999... */
            double x = 0, f = modf(xpRight->data.doubleVal, &x) + 1e-9;
            (*ver)[0] = (int)x;
            for (int i = 1; i < MAX_SEMVER_STR_LENGTH; ++i) {
                f *= 10;
                double decimal = modf(f, &x);
                if (fabs(decimal) < 1e-5) {
                    (*ver)[1] = (int)x;
                    return true;
                }
            }
            return false;
        }
        case DataString:
            return parseSemVer(xpRight->data.stringVal, ver);
        default:
            break;
    }
    return false;
}

/* Convert an Expr node to Context object with customized data type. */
Context *newContextFromExpr(Expr *xpRight, char *typeName) {
    Context *ctx = newContext();
    if (!ctx) {
        return NULL;
    }
    assert(isValidIdentifier(typeName));
    ctx->customDataType = strconcat(1, typeName);
    ctx->dataType = DataCustom;
    ctx->data.boolVal = xpRight->data.boolVal;
    ctx->data.doubleVal = xpRight->data.doubleVal;
    ctx->data.intVal = xpRight->data.intVal;
    ctx->data.stringVal = strconcat(1, xpRight->data.stringVal);
    return ctx;
}

/**
 * Binary Operation, always try to cast right side data to left and
 */
bool evalBinaryOpCtx(Expr *xpLeft, Expr *xpRight, ExprList *xpList, ContextExtension *extPtr, int op, int *err) {
    assert(xpLeft != NULL);
    assert(xpRight != NULL || xpList != NULL || extPtr != NULL);
    LURE_LOG("");
    switch (getDataType(xpLeft)) {
        case DataBool: {
            bool left = getBooleanData(xpLeft);
            bool right = typeCastBool(xpRight, err);
            switch (op) {
                case TK_AND:
                    return left && right;
                case TK_OR:
                    return left || right;
                case TK_EQ:
                case TK_GE:
                case TK_LE:
                    return left == right;
                case TK_NE:
                case TK_LT:
                case TK_GT:
                    return left != right;
                case TK_IN: {
                    for (int i = 0; i < xpList->nExpr; ++i) {
                        right = typeCastBool(xpList->list[i], err);
                        if (left == right) {
                            return true;
                        }
                    }
                    return false;
                }
                case TK_NOTIN:
                    return !evalBinaryOpCtx(xpLeft, xpRight, xpList, extPtr, TK_IN, err);
                default:
                    *err = Err_UnsupportedOp;
                    return false;
            }
            break;
        }
        case DataInt: {
            int left = getIntData(xpLeft);
            int right = typeCastInt(xpRight, err);
            switch (op) {
                case TK_AND:
                    return left && right;
                case TK_OR:
                    return left || right;
                case TK_LE:
                    return left <= right;
                case TK_GE:
                    return left >= right;
                case TK_LT:
                    return left < right;
                case TK_GT:
                    return left > right;
                case TK_NE:
                    return left != right;
                case TK_EQ:
                    return left == right;
                case TK_IN:
                    for (int i = 0; i < xpList->nExpr; ++i) {
                        right = typeCastInt(xpList->list[i], err);
                        if (left == right && *err != Err_BadInput) {
                            return true;
                        }
                    }
                    return false;
                case TK_NOTIN:
                    return !evalBinaryOpCtx(xpLeft, xpRight, xpList, extPtr, TK_IN, err);
                default:
                    *err = Err_UnsupportedOp;
                    return false;
            }
            break;
        }
        case DataDouble: {
            double left = getDoubleData(xpLeft);
            double right = typeCastDouble(xpRight, err);
            switch (op) {
                case TK_AND:
                    return left && right;
                case TK_OR:
                    return left || right;
                case TK_LE:
                    return left <= right;
                case TK_GE:
                    return left >= right;
                case TK_LT:
                    return left < right;
                case TK_GT:
                    return left > right;
                case TK_NE:
                    return left != right;
                case TK_EQ:
                    return left == right;
                case TK_IN:
                    for (int i = 0; i < xpList->nExpr; ++i) {
                        right = typeCastDouble(xpList->list[i], err);
                        if (left == right && *err != Err_BadInput) {
                            return true;
                        }
                    }
                    return false;
                case TK_NOTIN:
                    return !evalBinaryOpCtx(xpLeft, xpRight, xpList, extPtr, TK_IN, err);
                default:
                    *err = Err_UnsupportedOp;
                    return false;
            }
            break;
        }
        case DataString: {
            char *left = getStringData(xpLeft);
            char *right = newStringFromCast(xpRight, err);
            assert(left);
            int cmp = right == NULL ? 1 : strcmp(left, right);
            bool ret = false;
            switch (op) {
                case TK_AND:
                    ret = left && right;
                    break;
                case TK_OR:
                    ret = left || right;
                    break;
                case TK_LE:
                    ret = (left && right) && cmp <= 0;
                    break;
                case TK_GE:
                    ret = left && right && cmp >= 0;
                    break;
                case TK_LT:
                    ret = left && right && cmp < 0;
                    break;
                case TK_GT:
                    ret = left && right && cmp > 0;
                    break;
                case TK_NE:
                    ret = left && right && cmp != 0;
                    break;
                case TK_EQ:
                    ret = left && right && cmp == 0;
                    break;
                case TK_IN:
                    ret = left && xpList && hashmap_contains(xpList->map, left);
                    break;
                case TK_NOTIN:
                    ret = !evalBinaryOpCtx(xpLeft, xpRight, xpList, extPtr, TK_IN, err);
                    break;
                default:
                    *err = Err_UnsupportedOp;
                    ret = false;
                    break;
            }
            free(right);
            return ret;
        }
        case DataCustom: {
            if (extPtr == NULL) {
                extPtr = xpLeft->ext ? xpLeft->ext : xpRight->ext;
            }
            assert(extPtr != NULL);
            int cmp = extPtr->cmp(xpLeft, xpRight);
            switch (op) {
                case TK_AND:
                    return extPtr->toBool(xpLeft) && extPtr->toBool(xpRight);
                case TK_OR:
                    return extPtr->toBool(xpLeft) || extPtr->toBool(xpRight);
                case TK_LE:
                    return cmp <= 0;
                case TK_GE:
                    return cmp >= 0;
                case TK_LT:
                    return cmp < 0;
                case TK_GT:
                    return cmp > 0;
                case TK_NE:
                    return cmp != 0;
                case TK_EQ:
                    return cmp == 0;
                case TK_IN:
                    for (int i = 0; i < xpList->nExpr; ++i) {
                        if (extPtr->cmp(xpLeft, xpList->list[i]) == 0) {
                            return true;
                        }
                    }
                    return false;
                case TK_NOTIN:
                    return !evalBinaryOpCtx(xpLeft, xpRight, xpList, extPtr, TK_IN, err);
                default:
                    *err = Err_UnsupportedOp;
                    return false;
            }
        } break;
        default:
            break;
    }
    return false;
}

/**
 * Calculate root->derivedData, root->ext directly from extensions.
 * @param root (nonnull) is the Expr to derive
 * @param extMap (nonnull) is the extensions map
 * @param ctxData (nullable) is the data retrieved from the ctxMap
 * @return boolean value indicates that:
 *  (false) nothing is derived,
 *  (true) resolve succeededs
 */
bool resolveFromExtension(Expr *root, map_t extMap, char *extKey, DataValue *ctxData) {
    ContextExtension *ctxExtPtr = NULL;
    root->extKey = strconcat(1, extKey);
    int ok = hashmap_get(extMap, root->extKey, (void **)(&ctxExtPtr));
    if (ok != MAP_OK || ctxExtPtr == NULL) {
        return false;
    }
    root->ext = ctxExtPtr;
    return ctxExtPtr->derive(root, ctxData);
}

/**
 * Calculate root->derivedData, root->ext from map look up or function eval.
 * @param root (nonnull) is the Expr to derive
 * @param ctxMap (nonnull) is the contexts map
 * @param extMap (nonnull) is the extensions map
 * @return boolean value indicates that:
 *  (false) nothing is derived,
 *  (true) resolve succeeded
 */
bool resolveExpr(Expr *root, map_t ctxMap, map_t extMap) {
    assert(root && ctxMap && extMap && root->key);
    Context *ctxOfKey;
    int code = hashmap_get(ctxMap, root->key, (void **)(&ctxOfKey));
    /* Derive data only if Node Type is function or key exists in the context. */
    bool inCtx = !(code != MAP_OK || ctxOfKey == NULL || ctxOfKey->dataType == DataUnknown);
    bool isFunc = (root->type == EpFunction);
    if (inCtx || isFunc) {
        if (root->derivedValue) {
            free(root->derivedValue->stringVal);
            free(root->derivedValue);
            root->derivedValue = NULL;
        }
        root->derivedValue = (DataValue *)malloc(sizeof(DataValue));
    }
    /** resolving function has the highest priority. */
    if (isFunc) {
        return resolveFromExtension(root, extMap, getStringData(root), NULL);
    } else if (inCtx) {
        /** resolve from key-value context. */
        root->derivedDataType = ctxOfKey->dataType;
        switch (ctxOfKey->dataType) {
            case DataBool:
                root->derivedValue->boolVal = ctxOfKey->data.boolVal;
                return true;
            case DataInt:
                root->derivedValue->intVal = ctxOfKey->data.intVal;
                return true;
            case DataDouble:
                root->derivedValue->doubleVal = ctxOfKey->data.doubleVal;
                return true;
            case DataString:
                root->derivedValue->stringVal = strconcat(1, ctxOfKey->data.stringVal);
                return true;
            case DataCustom: {
                return resolveFromExtension(root, extMap, ctxOfKey->customDataType, &ctxOfKey->data);
            }
            default:
                break;
        }
    }
    return false;
}

/** evaluate expression tree with ctx, and track errors.
 * @param ctx a map of string to context objects
 * @param root expr node to process
 * @param err a pointer to retrieve error code
 * @param noDerive current root is final, no need to derive data
 * @return resulting value of the current expression
 */
bool evalInternal(map_t ctx, Expr *root, int *err, bool noDerive) {
    LURE_LOG("");
    if (root == NULL) {
        return false;
    }
    if (root->type == EpLiteral && root->dataType == DataBool) {
        return root->data.boolVal;
    }
    ContextExtension *ctxExtPtr = NULL;
    switch (root->type) {
        case EpLiteral: {
            switch (getDataType(root)) {
                case DataBool:
                    return getBooleanData(root);
                case DataInt:
                    return (bool)getIntData(root);
                case DataDouble:
                    return (bool)getDoubleData(root);
                case DataString: /* if non-empy first try to translate it into context. */
                {
                    if (root->data.stringVal == NULL || root->data.stringVal[0] == '\0') {
                        return false;
                    }
                    if (!noDerive) {
                        resolveExpr(root, ctx, extensions);
                        return evalInternal(ctx, root, err, true);
                    }
                    return boolOfString(getStringData(root));
                }
                case DataCustom:
                    if (root->ext == NULL || root->ext->toBool == NULL) {
                        return false;
                    }
                    return root->ext->toBool(root);
                default:
                    break;
            }
            break;
        }
        case EpFunction: {
            /* md5mod(USER_UUID, 100), {USER_UUID: 'abcdef'} */
            if (strcasecmp(root->data.stringVal, "md5mod") == 0 && root->pList != NULL) {
                assert(root->pList->nExpr == 2);
                char *ctxRef = root->pList->list[0]->data.stringVal;
                int amp = root->pList->list[1]->data.intVal;
                assert(ctxRef != NULL);
                assert(amp > 0);
                Context *ctxOfKey = NULL;
                hashmap_get(ctx, ctxRef, (void **)(&ctxOfKey));
                size_t uuidLen = strnlen(ctxOfKey->key, MAX_IDENTIFIER_LENGTH);
                if (ctxOfKey != NULL && ctxOfKey->key != NULL) {
                    root->derivedValue->intVal = (int)(md5str(ctxOfKey->key, uuidLen) % amp);
                    root->derivedDataType = DataInt;
                    return true;
                }
                *err = Err_BadInput;
                return false;
            }
            bool ok = resolveExpr(root, ctx, extensions);
            if (!ok || root->ext == NULL) {
                *err = Err_IrreducibleNode;
                return false;
            }
            return true;
        }
        case EpBinOp:
            if (root->pLeft == NULL || root->pLeft->key == NULL) {
                *err = Err_IrreducibleNode;
                return false;
            }
            /** Post order DFS, resolve children node before binop calc. */
            bool leftRet = evalInternal(ctx, root->pLeft, err, false);
            bool rightRet = evalInternal(ctx, root->pRight, err, false);
            switch (root->op) {
                case TK_AND:
                case TK_OR: {
                    return (root->op == TK_AND) ? (leftRet && rightRet) : (leftRet || rightRet);
                }
                case TK_BETWEEN:
                    /* should have been transformed into two binary ops*/
                    *err = Err_UnsupportedOp;
                    break;
                case TK_LE:
                case TK_GE:
                case TK_LT:
                case TK_GT:
                case TK_NE:
                case TK_EQ:
                case TK_IN:
                case TK_NOTIN: {
                    return evalBinaryOpCtx(root->pLeft, root->pRight, root->pList, ctxExtPtr, root->op, err);
                }
                default:
                    break;
            }
        default:
            break;
    }
    return false;
}

typedef struct yy_buffer_state *YY_BUFFER_STATE;
extern int yyparse(ExprList **rootExprList);
extern YY_BUFFER_STATE yy_scan_string(char *str);
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);

ExprList *compileList(char *s) {
    YY_BUFFER_STATE buffer = yy_scan_string(s);
    ExprList *rootList = NULL;
    yyparse(&rootList);
    yy_delete_buffer(buffer);
    return rootList;
}

/* compile string into expr, assuming there is only one expr. */
Expr *compile(char *s) {
    YY_BUFFER_STATE buffer = yy_scan_string(s);
    ExprList *rootList = NULL;
    yyparse(&rootList);
    yy_delete_buffer(buffer);
    Expr *xp = (rootList == NULL || rootList->list == NULL) ? NULL : rootList->list[0];
    free(rootList);
    return xp;
}
