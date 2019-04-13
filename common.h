#ifndef __LURE_COMMON_H__
#define __LURE_COMMON_H__

#define LOG_ERROR(msg) fprintf(stderr, msg)
#define PARAM_ASSERT(X) if(!(X))  LOG_ERROR("Invalid parameter, this must be a bug"); assert(X);
#endif