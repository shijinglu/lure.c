//
//  Created by Shijing Lu on 10/1/18.
//  Copyright © 2018 shijing. All rights reserved.
//

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "lure.h"
#include "lure_extension.h"

#define RED_FAILURE "\x1b[31m [FAILED] \x1b[0m"
#define GREEN_SUCCESS "\x1b[32m [SUCCESS] \x1b[0m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_RESET "\x1b[0m"

void expect(bool expectTrue, char *s, int nCtx, ...) {
    Expr *xp = compile(s);
    map_t ctxMap = hashmap_new();
    va_list args;
    va_start(args, nCtx);
    for (int i = 0; i < nCtx; ++i) {
        Context *ctx = va_arg(args, Context *);
        hashmap_put(ctxMap, ctx->key, ctx);
    }
    va_end(args);
    bool ret = eval(ctxMap, xp);
    if (ret != expectTrue) {
        fprintf(stderr, RED_FAILURE "'%s' expected: '%s', while actual is: %s\n", s, expectTrue ? "true" : "false",
                ret ? "true" : "false");
    } else {
        printf(GREEN_SUCCESS "'%s' is evaled correctly \n", s);
    }
    hashmap_free(ctxMap);
}

void test_case_sensitivity() {
    printf(ANSI_COLOR_YELLOW
           "\n################# Test Case Sensitivity "
           "#################\n" ANSI_COLOR_RESET);
    Context ctx;
    setIntContext(&ctx, "USER_ID", 123);
    expect(true, "USER_ID == 123", 1, &ctx);
    expect(false, "user_id == 123", 1, &ctx);
}

void test_int() {
    printf(ANSI_COLOR_YELLOW
           "\n################# Test Integer Value Context "
           "#################\n" ANSI_COLOR_RESET);
    Context ctx, ctx2;

    setIntContext(&ctx, "USER_ID", 123);
    expect(true, "USER_ID == 123", 1, &ctx);
    expect(true, "USER_ID >= 123", 1, &ctx);
    expect(true, "USER_ID <= 123", 1, &ctx);
    expect(true, "USER_ID >= 121", 1, &ctx);
    expect(true, "USER_ID >  121", 1, &ctx);
    expect(true, "USER_ID <  125", 1, &ctx);
    expect(true, "USER_ID != 125", 1, &ctx);

    expect(false, "USER_ID == 122", 1, &ctx);
    expect(false, "USER_ID >= 125", 1, &ctx);
    expect(false, "USER_ID <= 122", 1, &ctx);
    expect(false, "USER_ID >= 125", 1, &ctx);
    expect(false, "USER_ID >  125", 1, &ctx);
    expect(false, "USER_ID <  122", 1, &ctx);
    expect(false, "USER_ID != 123", 1, &ctx);

    expect(true, "USER_ID IN (122, 123, 124)", 1, &ctx);
    expect(true, "USER_ID NOT IN (122, 124, 125)", 1, &ctx);
    expect(true, "USER_ID between [123, 124]", 1, &ctx);
    expect(true, "USER_ID between [121, 123]", 1, &ctx);

    expect(false, "USER_ID NOT IN (122, 123, 124)", 1, &ctx);
    expect(false, "USER_ID IN (122, 124, 125)", 1, &ctx);
    expect(false, "USER_ID between [124, 125]", 1, &ctx);
    expect(false, "USER_ID between [120, 122]", 1, &ctx);

    setIntContext(&ctx2, "ZERO_INT", 0);
    expect(true, "USER_ID AND USER_ID", 2, &ctx, &ctx2);
    expect(true, "USER_ID OR ZERO_INT", 2, &ctx, &ctx2);
    expect(false, "USER_ID AND ZERO_INT", 2, &ctx, &ctx2);
    expect(false, "ZERO_INT AND ZERO_INT", 2, &ctx, &ctx2);

    expect(true, "(USER_ID == 123) && (ZERO_INT == 0)", 2, &ctx, &ctx2);
    expect(true, "USER_ID == 123 && ZERO_INT == 0", 2, &ctx, &ctx2);
    expect(true, "USER_ID == 123 || ZERO_INT == 1", 2, &ctx, &ctx2);
    expect(false, "USER_ID == 122 || ZERO_INT == 1", 2, &ctx, &ctx2);
}

void test_double() {
    printf(ANSI_COLOR_YELLOW
           "\n################# Test Double Value Context "
           "#################\n" ANSI_COLOR_RESET);
    Context ctx, ctx2;
    setDoubleContext(&ctx, "PI", 3.14);
    expect(true, "PI == 3.140", 1, &ctx);
    expect(true, "PI >= 3.140", 1, &ctx);
    expect(true, "PI <= 3.140", 1, &ctx);
    expect(true, "PI >  3.130", 1, &ctx);
    expect(true, "PI <  3.150", 1, &ctx);
    expect(true, "PI != 3.141", 1, &ctx);

    expect(false, "PI == 3.141", 1, &ctx);
    expect(false, "PI >= 3.141", 1, &ctx);
    expect(false, "PI <= 3.130", 1, &ctx);
    expect(false, "PI >  3.141", 1, &ctx);
    expect(false, "PI <  3.130", 1, &ctx);
    expect(false, "PI != 3.140", 1, &ctx);

    expect(true, "PI IN (3.130, 3.140, 3.150)", 1, &ctx);
    expect(true, "PI between [3.140, 3.150]", 1, &ctx);
    expect(true, "PI between [3.130, 3.140]", 1, &ctx);

    expect(false, "PI IN (3.120, 3.130, 3.150)", 1, &ctx);
    expect(false, "PI between [3.141, 3.150]", 1, &ctx);
    expect(false, "PI between [3.120, 3.130]", 1, &ctx);

    setDoubleContext(&ctx2, "ZERO_REAL", 0.0);
    expect(true, "PI AND PI", 2, &ctx, &ctx2);
    expect(true, "PI OR ZERO_REAL", 2, &ctx, &ctx2);
    expect(false, "PI AND ZERO_REAL", 2, &ctx, &ctx2);
    expect(false, "ZERO_REAL AND ZERO_REAL", 2, &ctx, &ctx2);

    expect(true, "(PI == 3.140) && (ZERO_REAL == 0)", 2, &ctx, &ctx2);
    expect(true, "PI == 3.140 && ZERO_REAL == 0", 2, &ctx, &ctx2);
    expect(true, "PI == 3.140 || ZERO_REAL == 1", 2, &ctx, &ctx2);
    expect(false, "PI == 3.141 || ZERO_REAL == 1", 2, &ctx, &ctx2);
}

void test_string() {
    printf(ANSI_COLOR_YELLOW
           "\n################# Test String Value Context "
           "#################\n" ANSI_COLOR_RESET);
    Context ctx, ctx2;
    setStringContext(&ctx, "USER_TAGS", "admin");
    expect(true, "USER_TAGS == admin", 1, &ctx);
    expect(true, "USER_TAGS >= admin", 1, &ctx);
    expect(true, "USER_TAGS <= admin", 1, &ctx);
    expect(true, "USER_TAGS >  admim", 1, &ctx);
    expect(true, "USER_TAGS <  admio", 1, &ctx);
    expect(true, "USER_TAGS != admi", 1, &ctx);

    expect(false, "USER_TAGS == admn", 1, &ctx);
    expect(false, "USER_TAGS >= admio", 1, &ctx);
    expect(false, "USER_TAGS <= admim", 1, &ctx);
    expect(false, "USER_TAGS >  admin", 1, &ctx);
    expect(false, "USER_TAGS <  admin", 1, &ctx);
    expect(false, "USER_TAGS != admin", 1, &ctx);

    expect(true, "USER_TAGS IN (admin, tester, fraudster)", 1, &ctx);
    expect(true, "USER_TAGS NOT IN (adm, admi, adminx, admin_)", 1, &ctx);
    expect(true, "USER_TAGS between [admin, admio]", 1, &ctx);
    expect(true, "USER_TAGS between [admim, admin]", 1, &ctx);

    expect(false, "USER_TAGS NOT IN (admin, tester, fraudster)", 1, &ctx);
    expect(false, "USER_TAGS IN (adm, admi, adminx, admin_)", 1, &ctx);
    expect(false, "USER_TAGS between [admio, admip]", 1, &ctx);
    expect(false, "USER_TAGS between [admia, admib]", 1, &ctx);

    setStringContext(&ctx2, "EMPTY_STR", "");
    expect(true, "USER_TAGS AND USER_TAGS", 2, &ctx, &ctx2);
    expect(true, "USER_TAGS OR EMPTY_STR", 2, &ctx, &ctx2);
    expect(false, "USER_TAGS AND EMPTY_STR", 2, &ctx, &ctx2);
    expect(false, "EMPTY_STR AND EMPTY_STR", 2, &ctx, &ctx2);

    expect(true, "(USER_TAGS == admin) && (EMPTY_STR == '')", 2, &ctx, &ctx2);
    expect(true, "USER_TAGS == admin && EMPTY_STR == ''", 2, &ctx, &ctx2);
    expect(true, "USER_TAGS == admin || EMPTY_STR == ''", 2, &ctx, &ctx2);
    expect(false, "USER_TAGS == admio || EMPTY_STR == 'x'", 2, &ctx, &ctx2);
}

void test_version() {
    printf(ANSI_COLOR_YELLOW
           "\n################# Test Eval With Version Type "
           "#################\n" ANSI_COLOR_RESET);
    Context ctx, ctx2;
    setCustomContext(&ctx, "APP_VERSION", "v3.2.1", "semver");
    expect(true, "APP_VERSION == v3.2.1", 1, &ctx);
    expect(true, "APP_VERSION >= v3.2.1", 1, &ctx);
    expect(true, "APP_VERSION <= v3.2.1", 1, &ctx);
    expect(true, "APP_VERSION >  v3.1.9", 1, &ctx);
    expect(true, "APP_VERSION <  v3.2.3", 1, &ctx);
    expect(true, "APP_VERSION != v3.2.10", 1, &ctx);

    expect(false, "APP_VERSION == v3.1.2", 1, &ctx);
    expect(false, "APP_VERSION >= v3.2.2", 1, &ctx);
    expect(false, "APP_VERSION <= v3.2.0", 1, &ctx);
    expect(false, "APP_VERSION >  v4.2.1", 1, &ctx);
    expect(false, "APP_VERSION <  v2.2.1", 1, &ctx);
    expect(false, "APP_VERSION != v3.2.1", 1, &ctx);

    expect(true, "APP_VERSION IN (v3.2.0, v3.2.1, v3.2)", 1, &ctx);
    expect(true, "APP_VERSION NOT IN (v3.2.0, v3.2, v3.1.2, v3.21)", 1, &ctx);
    expect(true, "APP_VERSION between [v3.2.0, v3.2.1]", 1, &ctx);
    expect(true, "APP_VERSION between [v3.2.1, v3.2.2]", 1, &ctx);

    expect(false, "APP_VERSION NOT IN (v3.2.0, v3.2.1, v3.2)", 1, &ctx);
    expect(false, "APP_VERSION IN (v3.2.0, v3.2, v3.1.2, v3.21)", 1, &ctx);
    expect(false, "APP_VERSION between [v3.1.9, v3.2.0]", 1, &ctx);
    expect(false, "APP_VERSION between [v3.2.9, v3.3.0]", 1, &ctx);

    setCustomContext(&ctx2, "ZERO_VERSION", "v0.0.0", "semver");
    expect(true, "APP_VERSION AND APP_VERSION", 2, &ctx, &ctx2);
    expect(true, "APP_VERSION OR ZERO_VERSION", 2, &ctx, &ctx2);
    expect(false, "APP_VERSION AND ZERO_VERSION", 2, &ctx, &ctx2);
    expect(false, "ZERO_VERSION AND ZERO_VERSION", 2, &ctx, &ctx2);
    expect(true, "ZERO_VERSION == v0.0.0", 2, &ctx, &ctx2);

    expect(true, "(APP_VERSION == v3.2.1) && (ZERO_VERSION == v0.0.0)", 2, &ctx, &ctx2);
    expect(true, "APP_VERSION == v3.2.1 && ZERO_VERSION == v0.0.0", 2, &ctx, &ctx2);
    expect(true, "APP_VERSION == v3.2.1 || ZERO_VERSION == v0.0.0", 2, &ctx, &ctx2);
    expect(false, "APP_VERSION == v3.2.0 || ZERO_VERSION == v0.1.0", 2, &ctx, &ctx2);
}

void test_mix() {
    Context ctxInt, ctxBool, ctxDouble, ctxString, ctxVer;
    setStringContext(&ctxString, "USER_TAGS", "admin");
    setIntContext(&ctxInt, "USER_ID", 123);
    setCustomContext(&ctxVer, "APP_VERSION", "v3.2.1", "semver");
    setDoubleContext(&ctxDouble, "PI", 3.14);
    setBoolContext(&ctxBool, "SWITCH", true);
    expect(true,
           "USER_TAGS IN (admin, tester) && USER_ID < 125 && APP_VERSION == v3.2.1 && PI == 3.14 && SWITCH == true", 5,
           &ctxBool, &ctxInt, &ctxDouble, &ctxString, &ctxVer);
}

int main(int argc, const char *argv[]) {
    // insert code here...
    install();
    test_memory();
    test_version();
    test_string();
    test_double();
    test_int();
    test_case_sensitivity();

    return 0;
}
