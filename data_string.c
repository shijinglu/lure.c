
#include <stdlib.h>
#include <assert.h>
#include <strings.h>
#include "data.h"
#include "util.h"

/* Release objects created in `NewStringData`*/
void CleanStringData(Data *self) {
    LURE_ASSERT(self != NULL, "self must not be NULL");
    free(self->raw.stringVal);
    self->raw.stringVal = NULL;
}

int StringDataCompareTo(Data *self, Data *rhs) {
    LURE_ASSERT(self != NULL, "self must not be NULL");
    LURE_ASSERT(rhs != NULL, "right hand side must not be NULL");
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
    LURE_ASSERT(self != NULL, "self must not be NULL");
    if (str_equal_ignore_case(self->raw.stringVal, "false")) {
        return false;
    }
    
    if ((self->raw.stringVal == NULL) || strlen(self->raw.stringVal) == 0) {
        return false;
    }
    return true;
}

int StringDataToInt(Data *self) {
    LURE_ASSERT(self != NULL, "self must not be NULL");
    return atoi(self->getCStr(self));
}

double StringDataToDouble(Data *self) {
    LURE_ASSERT(self != NULL, "self must not be NULL");
    return (double)self->raw.intVal;
}

char *StringDataToString(Data *self) {
    LURE_ASSERT(self != NULL, "self must not be NULL");
    return str_concat(1, self->raw.stringVal);
}

char *StringDataGetCStr(Data *self) {
    LURE_ASSERT(self != NULL, "self must not be NULL");
    return self->raw.stringVal;
}

Data *StringDataCopy(Data *self) {
    LURE_ASSERT(self != NULL, "self must not be NULL");
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

