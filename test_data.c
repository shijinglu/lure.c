#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include "data.h"

void test_bool_data() {
    Data * data = NewBoolData(false);
    assert(strcmp(data->getCStr(data), "false") == 0);
    assert(data->toBoolean(data) == false);
    assert(data->toInt(data) == 0);
    assert(data->toDouble(data) == 0.0);
    char *s = data->toString(data);
    assert(strcmp(s, "false") == 0);
    free(s); s = NULL;
    data->clean(data); free(data); data = NULL;
    
    data = NewBoolData(true);
    assert(strcmp(data->getCStr(data), "true") == 0);
    assert(data->toBoolean(data) == true);
    assert(data->toInt(data) == 1);
    assert(data->toDouble(data) == 1.0);
    s = data->toString(data);
    assert(strcmp(s, "true") == 0);
    free(s); s = NULL;
    data->clean(data); free(data); data = NULL;

    printf("[ok] test bool data\n");
}

void test_int_data() {
    Data *data = NewIntData(314);
    char *s = NULL;
    assert(data->toBoolean(data) == true);
    assert(data->toInt(data) == 314);
    assert(data->toDouble(data) == 314.0);
    s = data->toString(data);
    assert(strcmp(s, "314") == 0);
    free(s); s = NULL;
    data->clean(data); free(data); data = NULL;

    data = NewIntData(0);
    assert(data->toBoolean(data) == false);
    assert(data->toInt(data) == 0);
    assert(data->toDouble(data) == 0.0);
    s = data->toString(data);
    assert(strcmp(s, "0") == 0);
    free(s); s = NULL;
    data->clean(data); free(data); data = NULL;

    printf("[ok] test int data\n");
}

void test_double_data() {
    Data *data = NewDoubleData(3.14);
    char *s = data->toString(data);
    assert(data->toBoolean(data) == true);
    assert(data->toInt(data) == 3);
    assert(data->toDouble(data) == 3.14);
    assert(atof(s) == 3.14);
    data->clean(data); free(data); data = NULL;
    free(s); s = NULL;

    data = NewDoubleData(0.0);
    s = data->toString(data);
    assert(data->toBoolean(data) == false);
    assert(data->toInt(data) == 0);
    assert(data->toDouble(data) == 0.0);
    assert(atof(s) == 0.0);
    data->clean(data); free(data); data = NULL;
    free(s); s = NULL;
    
    printf("[ok] test double data \n");
}

void leak_test() {
    for (int i=0; i<100000; i++) {
        usleep(100);
        test_bool_data();
        test_int_data();
        test_double_data();
    }
}

int main(int argc, char **argv) {
//    leak_test();
    test_bool_data();
    test_int_data();
    test_double_data();
}
