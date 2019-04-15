
#include <stdlib.h>
#include <strings.h>
#include "data.h"
#include "util.h"
#include "logger.h"

/* Release objects created in `NewBoolData`*/
void CleanBoolData(Data *self) {
    /* dealloc singleton is not allowed. */
    LURE_ASSERT(self != NULL, "self must not be NULL");
    free(self->intKey); self->intKey = NULL;
}

int BoolDataCompareTo(Data *self, Data *rhs) {
    LURE_ASSERT(self != NULL, "self must not be NULL");
    LURE_ASSERT(rhs != NULL, "right hand side must not be NULL");
    return self->toInt(self) - rhs->toInt(rhs);
}

bool BoolDataToBool(Data *self) {
    LURE_ASSERT(self != NULL, "self must not be NULL");
    return (bool)self->raw.boolVal;
}

int BoolDataToInt(Data *self) {
    LURE_ASSERT(self != NULL, "self must not be NULL");
    return self->raw.boolVal ? 1 : 0;
}

double BoolDataToDouble(Data *self) {
    LURE_ASSERT(self != NULL, "self must not be NULL");
    return self->raw.boolVal ? 1.0 : 0.0;
}

char *BoolDataToString(Data *self) {
    LURE_ASSERT(self != NULL, "self must not be NULL");
    return self->raw.boolVal ? str_concat(1, "true") : str_concat(1, "false");
}

char *BoolDataGetCstr(Data *self) {
    LURE_ASSERT(self != NULL, "self must not be NULL");
    return self->raw.boolVal ? "true" : "false";
}

Data *BoolDataCopy(Data *self) {
    LURE_ASSERT(self != NULL, "self must not be NULL");
    return NewBoolData(self->raw.boolVal);
}

/* Create a new Data that manages integer and setup function ptrs accordingly. */
Data *NewBoolData(bool val) {
    Data *ptr = (Data *)calloc(1, sizeof(Data));
    ptr->raw.boolVal = val;
    ptr->rawType = RawDataBool;
    ptr->clean = CleanBoolData;
    ptr->compareTo = BoolDataCompareTo;
    ptr->toBoolean = BoolDataToBool;
    ptr->toDouble = BoolDataToDouble;
    ptr->toInt = BoolDataToInt;
    ptr->toString = BoolDataToString;
    ptr->getCStr = BoolDataGetCstr;
    ptr->copy = BoolDataCopy;
    ptr->intKey = BoolDataToString(ptr);
    return ptr;
}

Data * GetBoolDataFalse() {
    static Data * _BOOL_DATA_FALSE = NULL;
    if (_BOOL_DATA_FALSE == NULL) {
        _BOOL_DATA_FALSE = NewBoolData(false);
    }
    return _BOOL_DATA_FALSE;
}

Data * GetBoolDataTrue() {
    static Data * _BOOL_DATA_TRUE = NULL;
    if (_BOOL_DATA_TRUE == NULL) {
        _BOOL_DATA_TRUE = NewBoolData(true);
    }
    return _BOOL_DATA_TRUE;
}
