#include <stdint.h>
#include <string.h>

#if !defined(__LURE_MD5HASH_H__)
#define __LURE_MD5HASH_H__

typedef unsigned char u8;
typedef unsigned long long u64;
typedef unsigned int u32;

u64 md5(u8 *initial_msg, size_t initial_len);

u64 md5str(char *initial_msg, size_t initial_len);
#endif // __LURE_MD5HASH_H__
