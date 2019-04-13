
#include <stdlib.h>
#include <assert.h>
#include <strings.h>
#include "data.h"
#include "util.h"

/* Release objects created in `NewStringData`*/
void CleanStringData(Data *self) {
    free(self->raw.stringVal);
    self->raw.stringVal = NULL;
}

int StringDataCompareTo(Data *self, Data *rhs) {
    assert(self != NULL);
    assert(rhs != NULL);
    char *left = self->toString(self);
    char *right = rhs->toString(rhs);
    int cmp = strcmp(left, right);
    free(left);
    free(right);
    return cmp;
}

/* (boo)aString is false only if aString is literally "false"
 * (case insensitive) or it is empty. */
bool StringDataToBool(Data *self) {
    assert(self != NULL);

    if (str_equal_ignore_case(self->raw.stringVal, "false")) {
        return false;
    }
    return self->raw.stringVal != NULL;
}

int StringDataToInt(Data *self) {
    assert(self != NULL);
    return atoi(self->getCStr(self));
}

double StringDataToDouble(Data *self) {
    assert(self != NULL);
    return (double)self->raw.intVal;
}

char *StringDataToString(Data *self) {
    assert(self != NULL);
    return str_concat(1, self->raw.stringVal);
}

char *StringDataGetCStr(Data *self) {
    assert(self != NULL);
    return self->raw.stringVal;
}

Data *StringDataCopy(Data *self) {
    return NewStringData(self->raw.stringVal);
}

/* Create a new Data that manages string and setup function ptrs accordingly. */
Data *NewStringData(const char *val) {
    Data *ptr = (Data *)calloc(1, sizeof(Data));
    ptr->rawType = RawDataString;
    ptr->raw.stringVal = str_concat(1, val);
    ptr->clean = CleanStringData;
    ptr->compareTo = StringDataCompareTo;
    ptr->toBoolean = StringDataToBool;
    ptr->toDouble = StringDataToDouble;
    ptr->toInt = StringDataToInt;
    ptr->toString = StringDataToString;
    ptr->getCStr = StringDataGetCStr;
    ptr->copy = StringDataCopy;
    ptr->intKey = NULL;
    return ptr;
}

