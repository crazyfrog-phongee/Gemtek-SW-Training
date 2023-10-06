#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>

#include "config.h"
#include "errmacros.h"
#include "file_handle.h"

/* Global Variables */
static pthread_mutex_t glob_mutex;
static sem_t sem1;
static sem_t sem2;
static volatile int glob;

int random_int(int min, int max)
{
    return (rand() % (max - min + 1)) + min;
}

void init_semaphore()
{
    sem_init(&sem1, 0, 1);
    sem_init(&sem2, 0, 1);
}

void free_semaphore()
{
    sem_destroy(&sem1);
    sem_destroy(&sem2);
}

void *thread_handler(void *arg)
{
    thread_attr_t thread_attr = *((thread_attr_t *)arg);
    int ret;
    int rand;
    char filename[FILENAME_MAX] = {0};
    char send_msg[MSG_SIZE] = {0};

    for (int i = 0; i < thread_attr.loops; i++)
    {
        memset(filename, 0, strlen(filename));
        rand = random_int(1, 2);
        sprintf(filename, "output%d.txt", rand);

        switch (rand)
        {
        case 1:
            sem_wait(&sem1);

            ret = pthread_mutex_lock(&glob_mutex); /* Access critical resource */
            SYSCALL_ERROR(ret);
            glob++;
            sprintf(send_msg, "Thread %ld    |   %d\n", thread_attr.thread_id, glob);
            pthread_mutex_unlock(&glob_mutex); /* Back critical resource */
            SYSCALL_ERROR(ret);

            file_handle_write(filename, send_msg);
            sem_post(&sem1);
            break;

        case 2:
            sem_wait(&sem2);

            ret = pthread_mutex_lock(&glob_mutex); /* Access critical resource */
            SYSCALL_ERROR(ret);
            glob++;
            sprintf(send_msg, "Thread %ld    |   %d\n", thread_attr.thread_id, glob);
            pthread_mutex_unlock(&glob_mutex); /* Back critical resource */
            SYSCALL_ERROR(ret);

            file_handle_write(filename, send_msg);
            sem_post(&sem2);
            break;

        default:
            break;
        }
    }

    pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
    thread_attr_t threads_attr[NUM_THREADS];
    int ret;

    /* Init mutex, semaphore */
    pthread_mutex_init(&glob_mutex, NULL);
    init_semaphore();

    for (int i = 0; i < NUM_THREADS; i++)
    {
        threads_attr[i].loops = 5;
        threads_attr[i].thread_id = i;
        ret = pthread_create(&(threads_attr[i].pthread), NULL, &thread_handler, &(threads_attr[i]));
        SYSCALL_ERROR(ret);
        printf("Thread %d created successfully\n", i + 1);
    }

    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads_attr[i].pthread, NULL);
    }

    /* Free memory and destroy mutex, semaphore */
    pthread_mutex_destroy(&glob_mutex);
    free_semaphore();

    return 0;
}
