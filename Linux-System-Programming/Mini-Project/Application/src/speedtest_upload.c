#define _DEFAULT_SOURCE /* usleep() */
#include <sys/select.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>

#include "speedtest_upload.h"
#include "http.h"
#include "handle_time.h"
#include "common.h"

float start_ul_time, stop_ul_time;
static pthread_mutex_t *total_ul_size_mutex;
extern long int total_ul_size;
extern int thread_all_stop;
static thread_t *thread;

void *calculate_ul_speed_thread(void *arg)
{
    double ul_speed = 0.0, duration = 0;
    while (1)
    {
        stop_ul_time = get_uptime();
        duration = stop_ul_time - start_ul_time;
        // ul_speed = (double)total_ul_size/1024/1024/duration*8;
        ul_speed = (double)total_ul_size / 1000 / 1000 / duration * 8;
        if (duration > 0)
        {
            printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\bUpload speed: %0.2lf Mbps", ul_speed);
            fflush(stdout);
        }
        usleep(500000);

        if (thread_all_stop)
        {
            stop_ul_time = get_uptime();
            duration = stop_ul_time - start_ul_time;
            // ul_speed = (double)total_ul_size/1024/1024/duration*8;
            ul_speed = (double)total_ul_size / 1000 / 1000 / duration * 8;
            if (duration)
            {
                printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\bUpload speed: %0.2lf Mbps", ul_speed);
                printf("\n");
                fflush(stdout);
            }
            break;
        }
    }

    pthread_exit(NULL);
}

void *upload_thread(void *arg)
{
    const char *TAG = "UPLOAD_THREAD";
    char data_log[200] = {0};

    thread_t *t_arg = arg;
    int i = t_arg->thread_index;

    int fd;
    char data[UL_BUFFER_SIZE] = {0};
    char sbuf[512] = {0};
    int size = 0;

    struct timeval tv;
    fd_set fdSet;
    int ret;

    memset(data, 0, sizeof(char) * UL_BUFFER_SIZE);

    fd = init_connection(&(thread[i].servinfo));
    error_unless(fd > 0, "Could not make connection to '%s'", thread[i].domain_name);

    build_http_post(sbuf, thread[i].request_url, thread[i].domain_name, sizeof(data) * UL_BUFFER_TIMES);
    
    ret = make_request(fd, sbuf);

    pthread_mutex_lock(total_ul_size_mutex);
    total_ul_size += size;
    pthread_mutex_unlock(total_ul_size_mutex);

    for (int j = 0; j < UL_BUFFER_TIMES; j++)
    {
        if ((size = send(fd, data, sizeof(data), 0)) != sizeof(data))
        {
            printf("Can't send data to server\n");
            goto error;
        }

        pthread_mutex_lock(total_ul_size_mutex);
        total_ul_size += size;
        pthread_mutex_unlock(total_ul_size_mutex);

        if (thread_all_stop)
        {
            goto error;
        }
    }

    while (1)
    {
        FD_ZERO(&fdSet);
        FD_SET(fd, &fdSet);

        tv.tv_sec = 3;
        tv.tv_usec = 0;
        int status = select(fd + 1, &fdSet, NULL, NULL, &tv);

        int recv_byte = recv(fd, sbuf, sizeof(sbuf), 0);
        if (status > 0 && FD_ISSET(fd, &fdSet))
        {
            if (recv_byte < 0)
            {
                printf("Can't receive data!\n");
                break;
            }
            else if (recv_byte == 0)
            {
                break;
            }
        }
    }
    printf("Thread[%d]: Exit", thread[i].thread_index);
    
error:
    if (fd)
        close(fd);
    thread[i].running = 0;
    return NULL;
}

int speedtest_upload(server_data_t *nearest_server, thread_t *thr, pthread_mutex_t *mtx)
{
    const char *TAG = "SPEEDTEST_DOWNLOAD";
    total_ul_size_mutex = mtx;
    thread = thr;

    int i;
    char dummy[128] = {0}, request_url[128] = {0};
    sscanf(nearest_server->url, "http://%[^/]/%s", dummy, request_url);

    start_ul_time = get_uptime();
    while (1)
    {
        for (i = 0; i < THREAD_NUMBER; i++)
        {
            memcpy(&thread[i].servinfo, &nearest_server->servinfo, sizeof(struct sockaddr_in));
            memcpy(&thread[i].domain_name, &nearest_server->domain_name, sizeof(nearest_server->domain_name));
            memcpy(&thread[i].request_url, request_url, sizeof(request_url));
            if (thread[i].running == 0)
            {
                thread[i].thread_index = i;
                thread[i].running = 1;
                pthread_create(&thread[i].tid, NULL, upload_thread, &thread[i]);
            }
        }

        if (thread_all_stop)
        {
            break;
        }
    }

    for (int i = 0; i < THREAD_NUMBER; i++)
    {
        pthread_join(thread[i].tid, NULL);
    }

#if (DEBUG_LVL > 0)
    LOG(TAG, "Exit speedtest_upload thread");
#endif

    return 0;
}