#ifndef CONFIG_H_
#define CONFIG_H_

#define NUM_THREADS 3
#define MSG_SIZE    255
#include <pthread.h>

typedef struct thread_attr
{
    pthread_t pthread;
    unsigned long int thread_id;
    int loops;
} thread_attr_t;

#endif