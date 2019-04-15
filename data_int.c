
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
    LURE_ASSERT(self != NULL, "self must not be NULL");
    LURE_ASSERT(rhs != NULL, "right hand side must not be NULL");
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
    LURE_ASSERT(self != NULL, "self must not be NULL");
    return (bool)self->raw.intVal;
}

int IntDataToInt(Data *self) {
    LURE_ASSERT(self != NULL, "self must not be NULL");
    return self->raw.intVal;
}

double IntDataToDouble(Data *self) {
    LURE_ASSERT(self != NULL, "self must not be NULL");
    return (double)self->raw.intVal;
}

char *IntDataToString(Data *self) {
    LURE_ASSERT(self != NULL, "self must not be NULL");
    char * num = (char *)calloc(64, sizeof(char));
    snprintf(num, 63, "%d", self->raw.intVal);
    num[63] = '\0';
    return num;
}

char *IntDataGetCStr(Data *self) {
    LURE_ASSERT(self != NULL, "self must not be NULL");
    return self->intKey;
}

Data *IntDataCopy(Data *self) {
    LURE_ASSERT(self != NULL, "self must not be NULL");
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

