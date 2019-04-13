#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "util.h"

void test_str_equal_ignore_case() {
    assert(str_equal_ignore_case(NULL, NULL) == true);
    assert(str_equal_ignore_case(NULL, "") == false);
    assert(str_equal_ignore_case("", "") == true);
    assert(str_equal_ignore_case("", "A") == false);
    assert(str_equal_ignore_case("a", "A") == true);
    assert(str_equal_ignore_case("中文", "中文") == true);
}

void test_str_concat() {
    char * s = str_concat(1, "hello world");
    assert(strcmp(s, "hello world") == 0);
    free(s);
}

void test_str_concat_large() {
    char *s = NULL;
    for (int i=0; i<100000; i++) {
        usleep(100);
        s = str_concat(2, "hello", i % 2 == 0 ? "world" : "there, how are you");
        free(s); s = NULL;
    }
}

int main(int argc, char **argv) {
    test_str_equal_ignore_case();
    test_str_concat();
    // test_str_concat_large();
}
