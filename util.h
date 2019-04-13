#ifndef __LURE_UTIL_H__
#define __LURE_UTIL_H__

#include <stdarg.h>

/* Case in sensitive comparator */
bool str_equal_ignore_case(const char *s1, const char *s2);

char *str_concat(int num_args, ...);


#endif // __LURE_UTIL_H__
