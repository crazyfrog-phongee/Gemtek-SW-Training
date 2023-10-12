/*
 * common.h
 *
 * Created on: Oct 12, 2023
 * Author: PhongEE
 */

#ifndef COMMON_H_
#define COMMON_H_

#ifndef DEBUG_LVL
#define debug(M, ...)
#else
#define debug(M, ...) fprintf(stderr, "DEBUG %s (in function '%s'):%d:  " M "\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#endif

#define jump_unless(A) \
    if (!(A))          \
    {                  \
        goto error;    \
    }

#define error_unless(A, M, ...)                 \
    if (!(A))                                   \
    {                                           \
        fprintf(stderr, M "\n", ##__VA_ARGS__); \
        goto error;                             \
    }

void LOG(const char *TAG, char *data);
void ftoa(double n, char *res, int afterpoint);

#endif /* COMMON_H_ */