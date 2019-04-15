
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <strings.h>
#include "data.h"
#include "util.h"

/* Release objects created in `NewIntData`*/
void CleanDoubleData(Data *self) {
    free(self->intKey);
    self->intKey = NULL;
}

int DoubleDataCompareTo(Data *self, Data *rhs) {
    LURE_ASSERT(self != NULL, "self must not be NULL");
    LURE_ASSERT(rhs != NULL, "right hand side must not be NULL");
    double left = self->toDouble(self);
    double right = rhs->toDouble(rhs);
    if (left < right) {
        return -1;
    } else if (left > right) {
        return 1;
    } else {
        return 0;
    }
}

bool DoubleDataToBool(Data *self) {
    LURE_ASSERT(self != NULL, "self must not be NULL");
    return (bool)self->raw.doubleVal;
}

int DoubleDataToInt(Data *self) {
    LURE_ASSERT(self != NULL, "self must not be NULL");
    return (int)self->raw.doubleVal;
}

double DoubleDataToDouble(Data *self) {
    LURE_ASSERT(self != NULL, "self must not be NULL");
    return self->raw.doubleVal;
}

char * DoubleDataToString(Data *self) {
    LURE_ASSERT(self != NULL, "self must not be NULL");
    char * num = (char *)calloc(64, sizeof(char));
    snprintf(num, 63, "%lf", self->raw.doubleVal);
    num[63] = '\0';
    return num;
}

char * DoubleDataGetCstr(Data *self) {
    LURE_ASSERT(self != NULL, "self must not be NULL");
    return self->intKey;
}

Data *DoubleDataCopy(Data *self) {
    LURE_ASSERT(self != NULL, "self must not be NULL");
    return NewDoubleData(self->toDouble(self));
}

/* Create a new Data that manages integer and setup function ptrs accordingly. */
Data *NewDoubleData(double val) {
    Data *ptr = (Data *)calloc(1, sizeof(Data));
    ptr->raw.doubleVal = val;
    ptr->rawType = RawDataDouble;
    ptr->clean = CleanDoubleData;
    ptr->compareTo = DoubleDataCompareTo;
    ptr->toBoolean = DoubleDataToBool;
    ptr->toDouble = DoubleDataToDouble;
    ptr->toInt = DoubleDataToInt;
    ptr->toString = DoubleDataToString;
    ptr->getCStr = DoubleDataGetCstr;
    ptr->copy = DoubleDataCopy;
    ptr->intKey = DoubleDataToString(ptr);
    return ptr;
}

