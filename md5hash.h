#include <stdint.h>
#include <string.h>

#if !defined(__LURE_MD5HASH_H__)
#define __LURE_MD5HASH_H__

uint64_t md5(uint8_t *initial_msg, size_t initial_len);

uint64_t md5str(char *initial_msg, size_t initial_len);
#endif // __LURE_MD5HASH_H__
