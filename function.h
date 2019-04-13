#ifndef __LURE_FUNCTION_H__
#define __LURE_FUNCTION_H__

#include "data.h"

typedef struct FunctionS {
    char *name;
    void *meta_data;
    Data *(*derive)(Data **params, int nparams);
} FunctionType;

FunctionType *NewMD5ModFunction(void);

/* This function is meant to be initializer and is not thread safe. */
void function_install(FunctionType *f);
Data *function_derive(char *fname, Data **params, int nparams);

#endif // __LURE_FUNCTION_H__
