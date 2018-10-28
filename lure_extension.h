#if !defined(__LURE_EXTENSION_H__)
#define __LURE_EXTENSION_H__

#include <stdbool.h>
#include "hashmap.h"

typedef struct Expr Expr;
typedef struct ContextExtension ContextExtension;
typedef union LureDataT DataValue;

extern map_t extensions; /* a global variable holding extensions. */

struct ContextExtension {
    /** Extension name in the LURE expression or in the Context.customDataType. */
    char *key;

    /* for bool casting, like a&&b a||b etc. */
    bool (*toBool)(Expr *expr);

    /* Define this method to derive data from exsiting and additional data from context. */
    bool (*derive)(Expr *expr, DataValue *additionalData);

    /* Define this method to allow LURE to compare two sides of a binary operator. */
    int (*cmp)(Expr *left, Expr *right);

    /* If extension allocates data, de-allocate them here. */
    void (*customDataDealloc)(Expr *expr);
};

/* This should be run once to install extensions. */
void install();

#endif /* __LURE_EXTENSION_H__ */
