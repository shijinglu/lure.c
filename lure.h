#ifndef __LURE_H__
#define __LURE_H__

#include <stdbool.h>
#include "hashmap.h"

/* length limit for legic string literal (an identifier or a quoted string) */
#define MAX_IDENTIFIER_LENGTH 256

/* maximum length of a string that represents a sematic version, i.e. max version is 999999999. */
#define MAX_SEMVER_STR_LENGTH 32

/* default length of expression list. */
#define DEFAULT_EXPR_LIST_LENGTH 4

/** Forward struct defs*/
typedef struct Expr Expr;
typedef struct ExprList ExprList;
typedef struct Context Context;
typedef struct SemVer SemVer;
typedef struct ContextExtension ContextExtension;

/* All supported data types. */
typedef enum {
    DataUnknown = 0,
    DataBool,    /* bool */
    DataInt,     /* int */
    DataDouble,  /* double */
    DataString,  /* char* */
    DataVersion, /* char[3] */
    DataCustom   /* customized data type */
} DataType;

typedef enum {
    EpUnknown = 0, /* Reducible nodes */
    EpFunction,
    EpBinOp,
    EpUnaryOp,
    EpLiteral /* Literals, irreducibile leafs.  */
} EpType;

typedef union LureDataT {
    bool boolVal;
    int intVal;
    double doubleVal;
    char *stringVal;
    void *genericVal;
} DataValue;

struct Expr {
    /* non optional fields*/
    char *key;   /*description of cur expr, must not be NULL.*/
    EpType type; /* type of expr node. */
    DataValue data;
    DataType dataType;

    /* optional fields*/
    int op;          /* opcode such as TK_LE, TK_NUMBER etc */
    Expr *pLeft;     /* left node */
    Expr *pRight;    /* right node*/
    ExprList *pList; /* when op == TK_IN or TK_FUNCTION */

    char *extKey;             /* a string to retrieve extension, should be set only once. */
    ContextExtension *ext;    /* extension ptr, should be set only once. */
    DataValue *derivedValue;  /* derived value from map look up or function eval, this is mutable. */
    DataType derivedDataType; /* type of derived data type, this is set along with derivedValue. */
};

struct ExprList {
    int nExpr;
    char *key;
    int capacity;
    Expr **list;
    map_t map; /*map of Expr.key to Expr*/
};

struct Context {
    char *key;
    DataType dataType;
    char *customDataType; /* used for when data type is customized. */
    void *customData;     /* store casted customized data to avoid duplicatd casting. */
    DataValue data;
};

#define Err_EmptyNode 0x000001       /* Expr root is NULL */
#define Err_IrreducibleNode 0x000002 /* node is irreducible such as a hanging literal. */
#define Err_BadInput 0x000003        /* incorrect function inputs. */
#define Err_UnsupportedFunc 0x000004 /* function not supported */
#define Err_UnsupportedOp 0x000005   /* unsupported operator give context and values. */
#define Err_MemError 0x000006        /* memory error like out of mem etc. */

/************ FUNCTIONS ****************/
void yaclog(int i, char *msg);

/* parsing version to */
bool parseSemVer(char *s, int (*ver)[3]);

/* return literals of operator tokens.*/
char *descOfOp(int opcode);

/** Reducing functions */
ExprList *exprListOfExpr(Expr *xp);
ExprList *exprListAppend(ExprList *xpList, Expr *xp);
ExprList *newExprList();
void freeExprList(ExprList *xpList);

Expr *newExpr();
void freeExpr(Expr *xp);
Expr *exprLiteral(Expr *xpLiteral);
Expr *exprUnaryOp(int uop, Expr *xp);
Expr *exprBinOp(Expr *xp1, int bop, Expr *xp2);
Expr *exprBetween(Expr *xpLeft, Expr *xpLower, Expr *xpUpper);
Expr *exprFunction(char *fname, ExprList *xpList);
Expr *exprIn(Expr *xpLeft, int opInOrNot, ExprList *xpList);
Expr *exprOfInt(int n);
Expr *exprOfDouble(double n);
Expr *exprOfString(char *s);

/* value getters */
char *getStringData(Expr *expr);
bool getBooleanData(Expr *expr);
int getIntData(Expr *expr);
double getDoubleData(Expr *expr);
void *getGenericData(Expr *expr);

/* evaluate expression with ctx, return false in case of error. */
bool eval(map_t ctx, Expr *root);

/* evaluate expression with ctx, and track errors. */
bool evalInternal(map_t ctx, Expr *root, int *err, bool noDerive);

/* compile string into expr list (top most rule). */
ExprList *compileList(char *s);

/* compile string into expr, assuming there is only one expr. */
Expr *compile(char *s);

#endif
