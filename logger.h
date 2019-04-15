#ifndef __LURE_LOGGER_H__
#define __LURE_LOGGER_H__

#include <stdio.h>
#include <assert.h>

#define LURE_INFO(args...) \
fprintf(stdout, "[LURE INFO]: %s(%s:%d), ", __func__, __FILE__, __LINE__); \
fprintf(stdout, args); \
fprintf(stdout, "\n")

// #define LURE_LOG(msg, args...) \
// do { } while (0)

#define LURE_ERROR(args...) \
fprintf(stderr, "[LURE ERROR]: %s(%s:%d), ", __func__, __FILE__, __LINE__); \
fprintf(stderr, args); \
fprintf(stderr, "\n"); \
exit(-1)

#define LURE_ASSERT(condition, args...) \
if (!(condition)) { \
    fprintf(stderr, "[LURE ERROR]: %s(%s:%d), ", __func__, __FILE__, __LINE__); \
    fprintf(stderr, args); \
    fprintf(stderr, "\n"); \
    exit(-1); \
}

#endif /* __LURE_LOGGER_H__ */
