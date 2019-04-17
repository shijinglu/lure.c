
#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "node.h"
#include "lure.h"

#define RED_FAILURE "\x1b[31m [FAILED] \x1b[0m"
#define GREEN_SUCCESS "\x1b[32m [SUCCESS] \x1b[0m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_RESET "\x1b[0m"

map_t example_context() {
    static ContextPtr ctx = NULL;
    if (ctx == NULL) {
        ctx = NewContext();
        setIntContext(ctx, "USER_ID", 123);
        setIntContext(ctx, "ZERO_INT", 0);
        setDoubleContext(ctx, "PI", 3.14);
        setDoubleContext(ctx, "ZERO_REAL", 0.0);
        setStringContext(ctx, "USER_TAGS", "admin");
        setStringContext(ctx, "EMPTY_STR", "");
        setCustomContext(ctx, "APP_VERSION", "v3.2.1", "semver");
        setCustomContext(ctx, "ZERO_VERSION", "v0.0.0", "semver");
        setStringContext(ctx, "MESSAGE", "hello world");
        setBoolContext(ctx, "SWITCH", true);
        setStringContext(ctx, "environment", "Dev");
    }
    return ctx;
}

void expect(bool expectTrue, char *s, ContextPtr context) {
    bool ret = lure_eval(context, s);
    if (ret != expectTrue) {
        fprintf(stderr,
                RED_FAILURE "'%s' expected: '%s', while actual is: %s\n",
                s,
                expectTrue ? "true" : "false",
                ret ? "true" : "false");
    } else {
        printf(GREEN_SUCCESS "'%s' is evaled correctly \n", s);
    }
}

void test_lure_eval() {
    printf(ANSI_COLOR_YELLOW
           "\n################# Test Integer Value Context "
           "#################\n" ANSI_COLOR_RESET);
    ContextPtr ctx = example_context();
    expect(true, "environment == Dev", ctx);
    expect(true, "USER_ID == 123", ctx);
    expect(false, "user_id == 123", ctx);
    expect(true, "USER_ID == 123", ctx);
    expect(true, "USER_ID >= 123", ctx);
    expect(true, "USER_ID <= 123", ctx);
    expect(true, "USER_ID >= 121", ctx);
    expect(true, "USER_ID >  121", ctx);
    expect(true, "USER_ID <  125", ctx);
    expect(true, "USER_ID != 125", ctx);
    expect(false, "USER_ID == 122", ctx);
    expect(false, "USER_ID >= 125", ctx);
    expect(false, "USER_ID <= 122", ctx);
    expect(false, "USER_ID >= 125",  ctx);
    expect(false, "USER_ID >  125", ctx);
    expect(false, "USER_ID <  122",  ctx);
    expect(false, "USER_ID != 123", ctx);
    expect(true, "USER_ID IN (122, 123, 124)", ctx);
    expect(true, "USER_ID NOT IN (122, 124, 125)", ctx);
    expect(true, "USER_ID between 123 and 124", ctx);
    expect(true, "USER_ID between 121 and 123", ctx);
    expect(false, "USER_ID NOT IN (122, 123, 124)", ctx);
    expect(false, "USER_ID IN (122, 124, 125)", ctx);
    expect(false, "USER_ID between 124 and 125", ctx);
    expect(false, "USER_ID between 120 and 122", ctx);
    expect(true, "USER_ID && USER_ID", ctx);
    expect(true, "USER_ID || ZERO_INT", ctx);
    expect(false, "USER_ID && ZERO_INT", ctx);
    expect(false, "ZERO_INT && ZERO_INT", ctx);
    expect(true, "(USER_ID == 123) && (ZERO_INT == 0)", ctx);
    expect(true, "USER_ID == 123 && ZERO_INT == 0", ctx);
    expect(true, "(USER_ID == 123) || ZERO_INT == 1", ctx);
    expect(false, "(USER_ID == 122) || ZERO_INT == 1",ctx);
    expect(true, "PI == 3.140", ctx);
    expect(true, "PI >= 3.140", ctx);
    expect(true, "PI <= 3.140", ctx);
    expect(true, "PI >  3.130", ctx);
    expect(true, "PI <  3.150", ctx);
    expect(true, "PI != 3.141", ctx);
    expect(false, "PI == 3.141", ctx);
    expect(false, "PI >= 3.141", ctx);
    expect(false, "PI <= 3.130", ctx);
    expect(false, "PI >  3.141", ctx);
    expect(false, "PI <  3.130", ctx);
    expect(false, "PI != 3.140", ctx);
    expect(true, "PI IN (3.130, 3.140, 3.150)", ctx);
    expect(true, "PI between 3.140 AND 3.150", ctx);
    expect(true, "PI between 3.130 AND 3.140", ctx);
    expect(false, "PI IN (3.120, 3.130, 3.150)", ctx);
    expect(false, "PI between 3.141 AND 3.150", ctx);
    expect(false, "PI between 3.120 AND 3.130", ctx);
    expect(true, "PI && PI", ctx);
    expect(true, "PI || ZERO_REAL", ctx);
    expect(false, "PI && ZERO_REAL", ctx);
    expect(false, "ZERO_REAL && ZERO_REAL", ctx);
    expect(true, "(PI == 3.140) && (ZERO_REAL == 0)", ctx);
    expect(true, "(PI == 3.140) &&  ZERO_REAL == 0", ctx);
    expect(true, "(PI == 3.140) || ZERO_REAL == 1", ctx);
    expect(false, "(PI == 3.141) || ZERO_REAL == 1", ctx);
    expect(true, "USER_TAGS == admin", ctx);
    expect(true, "USER_TAGS >= admin", ctx);
    expect(true, "USER_TAGS <= admin", ctx);
    expect(true, "USER_TAGS >  admim", ctx);
    expect(true, "USER_TAGS <  admio", ctx);
    expect(true, "USER_TAGS != admi", ctx);
    expect(false, "USER_TAGS == admn", ctx);
    expect(false, "USER_TAGS >= admio", ctx);
    expect(false, "USER_TAGS <= admim", ctx);
    expect(false, "USER_TAGS >  admin", ctx);
    expect(false, "USER_TAGS <  admin", ctx);
    expect(false, "USER_TAGS != admin", ctx);
    expect(true, "USER_TAGS IN (admin, tester, fraudster)", ctx);
    expect(true, "USER_TAGS NOT IN (adm, admi, adminx, admin_)", ctx);
    expect(true, "USER_TAGS between admin AND admio", ctx);
    expect(true, "USER_TAGS between admim AND admin", ctx);
    expect(false, "USER_TAGS NOT IN (admin, tester, fraudster)", ctx);
    expect(false, "USER_TAGS IN (adm, admi, adminx, admin_)", ctx);
    expect(false, "USER_TAGS between admio and admip", ctx);
    expect(false, "USER_TAGS between admia and admib", ctx);
    expect(true, "USER_TAGS && USER_TAGS", ctx);
    expect(true, "USER_TAGS || EMPTY_STR", ctx);
    expect(false, "USER_TAGS && EMPTY_STR", ctx);
    expect(false, "EMPTY_ST && EMPTY_STR", ctx);
    expect(true, "(USER_TAGS == admin)  && (EMPTY_STR == '')", ctx);
    expect(true, "(USER_TAGS == admin) && EMPTY_STR == ''", ctx);
    expect(true, "(USER_TAGS == admin) || EMPTY_STR == ''", ctx);
    expect(false, "(USER_TAGS == admio) || EMPTY_STR == 'x'", ctx);
    expect(true, "APP_VERSION == `v3.2.1`", ctx);
    expect(true, "APP_VERSION >= `v3.2.1`", ctx);
    expect(true, "APP_VERSION <= `v3.2.1`", ctx);
    expect(true, "APP_VERSION >  `v3.1.9`", ctx);
    expect(true, "APP_VERSION <  `v3.2.3`", ctx);
    expect(true, "APP_VERSION != `v3.2.10`", ctx);
    expect(false, "APP_VERSION == `v3.1.2`", ctx);
    expect(false, "APP_VERSION >= `v3.2.2`", ctx);
    expect(false, "APP_VERSION <= `v3.2.0`", ctx);
    expect(false, "APP_VERSION >  `v4.2.1`", ctx);
    expect(false, "APP_VERSION <  `v2.2.1`", ctx);
    expect(false, "APP_VERSION != `v3.2.1`", ctx);
    expect(true, "APP_VERSION IN (v3.2.0, v3.2.1, v3.2.3)", ctx);
    expect(true, "APP_VERSION NOT IN (v3.2.0, v3.2, v3.1.2, v3.21)", ctx);
    expect(true, "APP_VERSION between `v3.2.0` AND `v3.2.1`", ctx);
    expect(true, "APP_VERSION between `v3.2.1` AND `v3.2.2`", ctx);
    expect(false, "APP_VERSION NOT IN (v3.2.0, v3.2.1, v3.2)", ctx);
    expect(false, "APP_VERSION IN (`v3.2.0`, `v3.2`, `v3.1.2`, `v3.21`)", ctx);
    expect(false, "APP_VERSION between `v3.1.9` and `v3.2.0`", ctx);
    expect(false, "APP_VERSION between `v3.2.9` and `v3.3.0`", ctx);
    expect(true, "APP_VERSION && APP_VERSION", ctx);
    expect(true, "APP_VERSION || ZERO_VERSION", ctx);
    expect(false, "APP_VERSION && ZERO_VERSION", ctx);
    expect(false, "ZERO_VERSION && ZERO_VERSION", ctx);
    expect(true, "ZERO_VERSION == `v0.0.0`", ctx);
    expect(true, "(APP_VERSION == `v3.2.1`) && (ZERO_VERSION == `v0.0.0`)", ctx);
    expect(true, "(APP_VERSION == `v3.2.1`) && ZERO_VERSION == `v0.0.0`", ctx);
    expect(true, "(APP_VERSION == `v3.2.1`) || ZERO_VERSION == `v0.0.0`", ctx);
    expect(false, "(APP_VERSION == `v3.2.0`) || ZERO_VERSION == `v0.1.0`", ctx);
    expect(false, "md5mod(`this text will be mod to 14`, 100) == 14", ctx);
}

void test_mem_leak() {
    for (int i=0; i<10000; i++) {
        usleep(1024);
        test_lure_eval();
    }
}

void test_context() {
    printContext(example_context());
}

int main(int argc, const char * argv[]) {
    //    test_lure_eval();
    // test_mem_leak();
    test_context();
    return 0;
}

#undef RED_FAILURE
#undef GREEN_SUCCESS
#undef ANSI_COLOR_YELLOW
#undef ANSI_COLOR_RESET
