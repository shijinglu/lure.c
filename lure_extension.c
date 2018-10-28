#include "lure_extension.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "lure.h"

map_t extensions;

/** Parse semetic version from string like "v1.2.3"
 * v1.2.3 => 1.2.3 [true]
 * v1..2..3 => 1.2.3 [true]
 * v1.2 => 1.2.0 [true]
 * v1 => 1.0.0 [true]
 * v0.1.alpha => 0.1.0 [true]
 */
bool parseVerStr(char *s, int (*ver)[3]) {
    if (s == NULL || ver == NULL) {
        return false;
    }
    (*ver)[0] = 0;
    (*ver)[1] = 0;
    (*ver)[2] = 0;
    size_t m = strnlen(s, MAX_SEMVER_STR_LENGTH);
    int vi = 0;
    for (int i = 0, j = 0; i < m && j <= m && vi < 3; j++) {
        if (s[i] == 'v' || s[i] == 'V') {
            j = i++;
        } else if (!isdigit(s[j])) {
            if (j > i) (*ver)[vi++] = atoi(&s[i]);
            i = j + 1;
        }
    }
    return vi > 0;
}

void semVerFreeDerived(Expr *expr) {
    if (expr->derivedValue != NULL) {
        free(expr->derivedValue);
    }
}

/** set derived data and type. */
bool semVerDerive(Expr *expr, DataValue *additionalData) {
    if (expr == NULL) {
        return false;
    }
    char *verStr = NULL;
    if (additionalData && additionalData->stringVal) {
        verStr = additionalData->stringVal;
    } else if (expr->data.stringVal) {
        verStr = expr->data.stringVal;
    }

    int ver[3];
    bool ok = parseSemVer(verStr, &ver);
    if (ok) {
        free(expr->derivedValue);
        expr->derivedValue = (DataValue *)malloc(sizeof(DataValue));
        int *customData = (int *)malloc(sizeof(int) * 3);
        if (customData) {
            customData[0] = ver[0];
            customData[1] = ver[1];
            customData[2] = ver[2];
            expr->derivedValue->genericVal = (void *)customData;
            expr->derivedDataType = DataCustom;
            return true;
        }
    }
    return false;
}

int semVerComparator(Expr *left, Expr *right) {
    if (left && !left->derivedValue) {
        semVerDerive(left, NULL);
    }
    if (right && !right->derivedValue) {
        semVerDerive(right, NULL);
    }
    int *leftVer = (int *)getGenericData(left);
    int *rightVer = (int *)getGenericData(right);
    if (leftVer == NULL && rightVer == NULL) {
        return 0;
    } else if (leftVer == NULL && rightVer != NULL) {
        return -1;
    } else if (leftVer != NULL && rightVer == NULL) {
        return 1;
    }
    for (int i = 0; i < 3; ++i) {
        if (leftVer[i] != rightVer[i]) {
            return leftVer[i] - rightVer[i];
        }
    }
    return 0;
}

bool semVerToBool(Expr *expr) {
    int *ver = (int *)getGenericData(expr);
    if (ver != NULL) {
        return (bool)(ver[0] + ver[1] + ver[2]);
    }
    semVerDerive(expr, NULL);
    if (expr->derivedValue == NULL) {
        return false;
    }
    ver = (int *)expr->derivedValue->genericVal;
    return ver == NULL ? false : (bool)(ver[0] + ver[1] + ver[2]);
}

static bool _once = false;
void install() {
    if (_once) {
        return;
    }
    _once = true;
    extensions = hashmap_new();

    /* Add extention to handle version*/
    ContextExtension *ctxExt = (ContextExtension *)malloc(sizeof(ContextExtension));
    ctxExt->toBool = semVerToBool;
    ctxExt->cmp = semVerComparator;
    ctxExt->customDataDealloc = semVerFreeDerived;
    ctxExt->derive = semVerDerive;
    ctxExt->key = "semver";
    hashmap_put(extensions, ctxExt->key, ctxExt);
}
