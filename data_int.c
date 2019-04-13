
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <strings.h>
#include "data.h"

/* Release objects created in `NewIntData`*/
void CleanIntData(Data *self) {
    free(self->intKey);
    self->intKey = NULL;
}

int IntDataCompareTo(Data *self, Data *rhs) {
    assert(self != NULL);
    assert(rhs != NULL);
    if (rhs->rawType == RawDataDouble) {
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
    return self->toInt(self) - rhs->toInt(rhs);
}

bool IntDataToBool(Data *self) {
    assert(self != NULL);
    return (bool)self->raw.intVal;
}

int IntDataToInt(Data *self) {
    assert(self != NULL);
    return self->raw.intVal;
}

double IntDataToDouble(Data *self) {
    assert(self != NULL);
    return (double)self->raw.intVal;
}

char *IntDataToString(Data *self) {
    assert(self != NULL);
    char * num = (char *)calloc(64, sizeof(char));
    snprintf(num, 63, "%d", self->raw.intVal);
    num[63] = '\0';
    return num;
}

char *IntDataGetCStr(Data *self) {
    return self->intKey;
}

Data *IntDataCopy(Data *self) {
    return NewIntData(self->toInt(self));
}

/* Create a new Data that manages integer and setup function ptrs accordingly. */
Data *NewIntData(int val) {
    Data *ptr = (Data *)calloc(1, sizeof(Data));
    ptr->raw.intVal = val;
    ptr->rawType = RawDataInt;
    ptr->clean = CleanIntData;
    ptr->compareTo = IntDataCompareTo;
    ptr->toBoolean = IntDataToBool;
    ptr->toDouble = IntDataToDouble;
    ptr->toInt = IntDataToInt;
    ptr->toString = IntDataToString;
    ptr->getCStr = IntDataGetCStr;
    ptr->copy = IntDataCopy;
    ptr->intKey = IntDataToString(ptr);
    return ptr;
}

