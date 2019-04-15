#ifndef __LURE_DATA_H__
#define __LURE_DATA_H__
#include <stdbool.h> 

// #define LURE_LOG(msg, args...) printf("[LURE LOG]: %s(%s:%d) %s \n",
// __func__,__FILE__, __LINE__, msg, ##args)
#define LURE_LOG(msg, args...) \
do {                       \
} while (0)

#define LURE_ERROR(msg, args...) \
printf("[LURE ERROR]: %s(%s:%d) %s \n", __func__,__FILE__, __LINE__, msg, ##args); \
exit(-1);

typedef struct Data Data;

typedef enum {
    RawDataBool, 
    RawDataInt,
    RawDataDouble,
    RawDataString,
    RawDataCustom
} RawDataType;

typedef union RawData {
    bool boolVal;
    int intVal;
    double doubleVal;
    char *stringVal;
    void *genericVal;
} RawData;

/**
 * Data is the result object returned from each Node.evaluate(...)
 * Value of Data should be immutable.
 */
struct Data {
    /* holds actual data. */
    RawData raw;

    /* type of the raw data */
    RawDataType rawType;
    
    /* string representation of the value. E.g 1-> "1", 3.14 -> 3.14 */
    char * intKey;

    /* key for extention look up. */
    char * extKey;

    /* function ptr to compare this data to another.
     * NOTE: this function assumes both self and rhs are non-null. */
    int (*compareTo) (Data *self, Data *rhs);

    /* cast self to a boolean*/
    bool (*toBoolean)(Data *self);

    /* cast self to integer */
    int (*toInt)(Data *self);

    /* cast self to double */
    double (*toDouble)(Data *self);

    /* cast to string, this creates a new char array that needs deallocation later. */
    char * (*toString)(Data *self);

    /* get the string representation, error out if non available. */
    char * (*getCStr)(Data *self);
    /* clear resources. call data->clean(data) before dealloc a data object
     * To make sure all internal resources are cleared. 
     */
    void (*clean)(Data *self);
    
    /* Create a deep copy of self. */
    Data *(*copy)(Data *self);
};

/* static final instances. */
Data * GetBoolDataFalse(void);
Data * GetBoolDataTrue(void);

Data *NewBoolData(bool val);
Data *NewDoubleData(double val);
Data *NewIntData(int val);
Data *NewStringData(const char *val);
Data *NewVersionData(const char *val);

#endif
