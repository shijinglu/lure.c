/**
 *  test evalBinaryOpCtx
 *
 *  */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "lure.h"
#include "y.tab.h"

bool evalBinaryOpCtx(Context *ctxOfKey, Expr *xp, int op, int *err);

void logCtx(Context *ctx) {
    switch (ctx->type) {
        case CtxUnknown:
            printf("unknown ctx");
            break;
        case CtxBool:
            printf("%s", ctx->data.boolVal ? "true" : "false");
            break;
        case CtxInt:
            printf("%d", ctx->data.intVal);
            break;
        case CtxDouble:
            printf("%lf", ctx->data.doubleVal);
            break;
        case CtxString:
            printf("%s", ctx->data.stringVal);
        case CtxVersion:
            printf("v%d.%d.%d", ctx->data.ver[0], ctx->data.ver[1], ctx->data.ver[2]);
        default:
            break;
    }
}

void expect(Context *ctxOfKey, Expr *xp, int op, int *err, bool expectRet) {
    *err = 0;
    bool actualRet = evalBinaryOpCtx(ctxOfKey, xp, op, err);
    if (actualRet == expectRet && *err == 0) {
        printf("[PASS] : ");
    } else {
        printf("[FAILED] :");
    }
    printf("Ctx: ");
    logCtx(ctxOfKey);
    printf(", xp: %s", xp->key);
    printf(", op: %s :=> \t", descOfOp(op));
    printf("expected: %s", expectRet ? "true" : "false");
    printf(" <-> actual: %s; error: %d", actualRet ? "true" : "false", *err);
    printf("\n");
}

void test_bool() {
    Context ctx;
    ctx.type = CtxBool;
    ctx.key = "IS_ADMIN";
    Expr *xp = newExpr();
    int err;

    /* false == false is true*/
    ctx.data.boolVal = false;
    xp->data.boolVal = false;
    xp->key = "false";
    expect(&ctx, xp, TK_EQ, &err, true);

    /* false == true is false*/
    ctx.data.boolVal = false;
    xp->data.boolVal = true;
    xp->key = "true";
    expect(&ctx, xp, TK_EQ, &err, false);

    /* false != false is true*/
    ctx.data.boolVal = false;
    xp->data.boolVal = false;
    xp->key = "false";
    expect(&ctx, xp, TK_NE, &err, false);

    /* false != true is true*/
    ctx.data.boolVal = false;
    xp->data.boolVal = true;
    xp->key = "true";
    expect(&ctx, xp, TK_NE, &err, true);

    /* true AND false is false */
    ctx.data.boolVal = true;
    xp->data.boolVal = false;
    xp->key = "false";
    expect(&ctx, xp, TK_AND, &err, false);

    /* true AND true is true*/
    ctx.data.boolVal = true;
    xp->data.boolVal = true;
    xp->key = "true";
    expect(&ctx, xp, TK_AND, &err, true);

    /* false OR false is false*/
    ctx.data.boolVal = false;
    xp->data.boolVal = false;
    xp->key = "false";
    expect(&ctx, xp, TK_OR, &err, false);

    /* false OR true is true*/
    ctx.data.boolVal = false;
    xp->data.boolVal = true;
    xp->key = "true";
    expect(&ctx, xp, TK_OR, &err, true);

    free(xp);
}

int main(int argc, char const *argv[]) {
    test_bool();
    return 0;
}
