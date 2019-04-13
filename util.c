
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "util.h"

bool str_equal_ignore_case(const char *s1, const char *s2) {
    if (s1 == NULL) {
        if (s2 == NULL) {
            return true;
        }
        return false;
    }
    if (strcmp(s1, s2) == 0) {
        return true;
    }
    size_t n1 = strlen(s1);
    size_t n2 = strlen(s2);
    if (n1 != n2) {
        return false;
    }
    char delta = 'A' - 'a';
    for (size_t i = 0; i < n1; ++i) {
        char c1 = s1[i];
        if (c1 >= 'A' && c1 <= 'Z') {
            c1 = c1 - delta;
        }
        char c2 = s2[i];
        if (c2 >= 'A' && c2 <= 'A') {
            c2 = c2 - delta;
        }
        if (c1 != c2) {
            return false;
        }
    }
    return true;
}

/**
 * Create a new string by concatenate all strings,
 * eg:`char *str = strconcat(3, "testing ", "this ", "thing");`
 * NOTE: returned string needs to be freed later.
 * */
char *str_concat(int num_args, ...) {
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
    return res;
}

