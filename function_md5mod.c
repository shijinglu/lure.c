#include <assert.h>
#include <stdlib.h>
#include "function.h"
#include "data.h"
#include "md5hash.h"

Data *FunctionMD5ModDerive(Data **params, int nparams) {
    LURE_ASSERT(nparams == 2, "md5mod accepts 2 parameters, %d are given", nparams);
    LURE_ASSERT(params != NULL, "parameters must not be empty");
    LURE_ASSERT(params[0] != NULL, "first parameter must not be NULL");
    LURE_ASSERT(params[1] != NULL, "second parameter must not be NULL");
    char *msg = params[0]->getCStr(params[0]);
    int   amp = params[1]->toInt(params[1]);
    size_t msg_len = strlen(msg);
    u64 md5 = md5str(params[0]->getCStr(params[0]), msg_len);
    return NewIntData(md5 % amp);
}

FunctionType *NewMD5ModFunction() {
    FunctionType *ft = (FunctionType *)calloc(1, sizeof(FunctionType));
    ft->derive = FunctionMD5ModDerive;
    ft->name = "md5mod";
    ft->meta_data = NULL;
    return ft;
}
