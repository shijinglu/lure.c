
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <strings.h>
#include <ctype.h>
#include "data.h"
#include "util.h"


typedef struct sem_ver {
    unsigned major;
    unsigned minor;
    unsigned patch;
} sem_ver_t;

Data *NewVersionDataOfVersion(sem_ver_t *version);
bool parse_version(const char *vs, sem_ver_t *version);

/* Release objects created in `NewVersionData`*/
void CleanVersionData(Data *self) {
    free(self->raw.genericVal);
    self->raw.genericVal = NULL;
    free(self->intKey);
    self->intKey = NULL;
}

int version_cmp(sem_ver_t *lhsVer, sem_ver_t *rhsVer) {
    if (lhsVer->major != rhsVer->major) {
        return lhsVer->major - rhsVer->major;
    }
    if (lhsVer->minor != rhsVer->minor) {
        return lhsVer->minor - rhsVer->minor;
    }
    if (lhsVer->patch != rhsVer->patch) {
        return lhsVer->patch - rhsVer->patch;
    }
    return 0;
}

int VersionDataCompareTo(Data *self, Data *rhs) {
    assert(self != NULL);
    assert(rhs != NULL);
    assert(self->rawType == RawDataCustom);
    assert(rhs->rawType == RawDataCustom || rhs->rawType == RawDataString);
    int cmp = 0;
    sem_ver_t *lhsVer = (sem_ver_t *)self->raw.genericVal;
    sem_ver_t *rhsVer = NULL;
    if (rhs->rawType == RawDataString) {
        rhsVer = (sem_ver_t *)calloc(1, sizeof(sem_ver_t));
        bool ok = parse_version(rhs->getCStr(rhs), rhsVer);
        if (!ok) {
            LURE_ERROR("Unable to parse version \n");
        }
        cmp = version_cmp(lhsVer, rhsVer);
        free(rhsVer);
    } else if (rhs->rawType == RawDataCustom && str_equal_ignore_case(rhs->extKey, "semver")) {
        rhsVer = (sem_ver_t *)rhs->raw.genericVal;
        cmp = version_cmp(lhsVer, rhsVer);
    }
    return cmp;
}

/* false only verion is 0.0.0 */
bool VersionDataToBool(Data *self) {
    assert(self != NULL);
    sem_ver_t *ver = (sem_ver_t *)self->raw.genericVal;
    return ver->major != 0 || ver->minor != 0 || ver->patch != 0;
}

/* return major value. */
int VersionDataToInt(Data *self) {
    assert(self != NULL);
    sem_ver_t *ver = (sem_ver_t *)self->raw.genericVal;
    return ver->major;
}

double VersionDataToDouble(Data *self) {
    assert(self != NULL);
    return (double)self->raw.intVal;
}

char *VersionDataToString(Data *self) {
    assert(self != NULL);
    assert(self->raw.genericVal != NULL);

    sem_ver_t *version = (sem_ver_t *)self->raw.genericVal;
    char * num = (char *)calloc(64, sizeof(char));
    snprintf(num, 63, "v%d.%d.%d", version->major, version->minor, version->patch);
    num[63] = '\0';
    return num;
}

char *VersionDataGetCstr(Data *self) {
    return self->intKey;
}

/* parse string representation of a sematic version to version struct.
 * a valid sem version string must start with "v" or "V" and should have
 * only three version fields: major, minor and path. 
 * Examples: v3.2.1, v0.0.0, V123.3456.100001
 * 
 * @return true if parse succeeded. Version values are undefined if parse 
 * failed.
 */
bool parse_version(const char *vs, sem_ver_t *version) {
    assert(version != NULL);
    assert(vs != NULL);
    size_t ns = strlen(vs);
    if (ns < 6) {
        return false;
    }
    if (vs[0] != 'v' && vs[0] != 'V') {
        return false;
    }

    int parsed = 0; // tracks how many int parsed
    for(size_t i = 1; i < ns; i++) {
        if (!isdigit(vs[i])) {
            continue;
        }
        int v = atoi(&vs[i]);
        switch (parsed)
        {
            case 0:
                version->major = v;
                break;
            case 1:
                version->minor = v;
                break;
            case 2:
                version->patch = v;
                break;
            default:
                break;
        }
        ++parsed;
    }
    return true;
}

Data *VersionDataCopy(Data *self) {
    assert(self->rawType == RawDataCustom);
    assert(self->raw.genericVal != NULL);
    
    sem_ver_t *version = (sem_ver_t *)calloc(1, sizeof(sem_ver_t));
    sem_ver_t *srcVer = (sem_ver_t *)self->raw.genericVal;
    version->major = srcVer->major;
    version->minor = srcVer->minor;
    version->patch = srcVer->patch;
    return NewVersionDataOfVersion(version);
}

Data *NewVersionDataOfVersion(sem_ver_t *version) {
    Data *ptr = (Data *)calloc(1, sizeof(Data));
    ptr->extKey = "semver";
    ptr->raw.genericVal = (void *)version;
    ptr->rawType = RawDataCustom;
    ptr->clean = CleanVersionData;
    ptr->compareTo = VersionDataCompareTo;
    ptr->toBoolean = VersionDataToBool;
    ptr->toDouble = VersionDataToDouble;
    ptr->toInt = VersionDataToInt;
    ptr->toString = VersionDataToString;
    ptr->getCStr = VersionDataGetCstr;
    ptr->copy = VersionDataCopy;
    ptr->intKey = VersionDataToString(ptr);
    return ptr;
}
/* Create a new Data that manages integer and setup function ptrs accordingly. */
Data *NewVersionData(const char *val) {
    sem_ver_t *version = (sem_ver_t *)malloc(sizeof(sem_ver_t));
    parse_version(val, version);
    return NewVersionDataOfVersion(version);
}

