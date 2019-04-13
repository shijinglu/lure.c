
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
    assert(self != NULL);
    assert(rhs != NULL);
    double left = self->toDouble(self);
    double right = rhs->toDouble(rhs);
    if (right < left) {
        return -1;
    } else if (right > left) {
        return 1;
    } else {
        return 0;
    }
}

bool DoubleDataToBool(Data *self) {
    assert(self != NULL);
    return (bool)self->raw.doubleVal;
}

int DoubleDataToInt(Data *self) {
    assert(self != NULL);
    return (int)self->raw.doubleVal;
}

double DoubleDataToDouble(Data *self) {
    assert(self != NULL);
    return self->raw.doubleVal;
}

char * DoubleDataToString(Data *self) {
    assert(self != NULL);
    char * num = (char *)calloc(64, sizeof(char));
    snprintf(num, 63, "%lf", self->raw.doubleVal);
    num[63] = '\0';
    return num;
}

char * DoubleDataGetCstr(Data *self) {
    return self->intKey;
}

Data *DoubleDataCopy(Data *self) {
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

