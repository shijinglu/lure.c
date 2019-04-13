#include <assert.h>
#include <stdlib.h>
#include "function.h"
#include "hashmap.h"

static map_t FUNCTIONS_MAP = NULL;

FunctionType *getFuncPtr(char *fname) {
    if (FUNCTIONS_MAP == NULL) {
        function_install(NULL);
    }
    FunctionType *f = NULL;
    int ok = hashmap_get(FUNCTIONS_MAP, fname, (void **)(&f));
    if (ok == MAP_MISSING || f == NULL) {
        return NULL;
    }
    return f;
}


void function_install(FunctionType *f) {
    if (FUNCTIONS_MAP == NULL) {
        FUNCTIONS_MAP = hashmap_new();
        /** build-in functions **/
        function_install(NewMD5ModFunction());
        if (f != NULL) {
            hashmap_put(FUNCTIONS_MAP, f->name, f);
        }
    } else if ( f!= NULL && getFuncPtr(f->name) == NULL) {
        hashmap_put(FUNCTIONS_MAP, f->name, f);
    }
}


Data *function_derive(char *fname, Data **params, int nparams) {
    return getFuncPtr(fname)->derive(params, nparams);
}
