#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <time.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

#include "speedtest_download.h"
#include "common.h"
#include "http.h"
#include "handle_time.h"

float start_dl_time, stop_dl_time;
static pthread_mutex_t *total_dl_size_mutex;
extern long int total_dl_size;
extern int thread_all_stop;
static thread_t *thread;

void *download_thread(void *arg)
{
    const char *TAG = "DOWNLOAD_THREAD";
    char data_log[200] = {0};

    thread_t *t_arg = arg;
    int i = t_arg->thread_index;

    int fd;
    char sbuf[256] = {0};
    char rbuf[DL_BUFFER_SIZE] = {0};
    struct timeval tv;
    fd_set read_fds;

    int ret;

    fd = init_connection(&(thread[i].servinfo));
    error_unless(fd > 0, "Could not make connection to '%s'", thread[i].domain_name);

    build_http_get(sbuf, thread[i].domain_name, thread[i].request_url);

    ret = make_request(fd, sbuf);
    error_unless(ret != -1, "Could not make connection to '%s'", thread[i].domain_name);
    while (1)
    {
        FD_ZERO(&read_fds);
        FD_SET(fd, &read_fds);

        tv.tv_sec = 3;
        tv.tv_usec = 0;
        int status = select(fd + 1, &read_fds, NULL, NULL, &tv);

        int recv_byte = recv(fd, rbuf, sizeof(rbuf), 0);
        if (status > 0 && FD_ISSET(fd, &read_fds))
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
            else
            {
                pthread_mutex_lock(total_dl_size_mutex);
                total_dl_size += recv_byte;
                pthread_mutex_unlock(total_dl_size_mutex);
            }

            if (thread_all_stop)
            {
                break;
            }
        }
    }

error:
    if (fd)
        close(fd);
    thread[i].running = 0;
    return NULL;
}

int speedtest_download(server_data_t *nearest_server, thread_t *thr, pthread_mutex_t *mtx)
{
    const char *TAG = "SPEEDTEST_DOWNLOAD";
    const char download_filename[64] = "random3500x3500.jpg"; // 23MB
    total_dl_size_mutex = mtx;
    thread = thr;
    char url[128] = {0}, request_url[128] = {0}, dummy[128] = {0}, buf[128];
    char *token = NULL;

    sscanf(nearest_server->url, "http://%[^/]/%s", dummy, request_url);
    strncpy(url, request_url, sizeof(request_url));
    memset(request_url, 0, sizeof(request_url));

    token = strtok(url, "/");
    while (token != NULL)
    {
        memset(buf, 0, sizeof(buf)); 
        strncpy(buf, token, strlen(token));

        // Change file name
        if (strstr(buf, "upload.") != NULL)
        {
            strcat(request_url, download_filename);
        }
        else
        {
            strcat(request_url, buf);
            strcat(request_url, "/");
        }
        token = strtok(NULL, "/");
    }

    /* Start downloading*/
    start_dl_time = get_uptime();
    while (1)
    {
        for (int i = 0; i < THREAD_NUMBER; i++)
        {
            memcpy(&thread[i].servinfo, &nearest_server->servinfo, sizeof(struct sockaddr_in));
            memcpy(&thread[i].domain_name, &nearest_server->domain_name, sizeof(nearest_server->domain_name));
            memcpy(&thread[i].request_url, request_url, sizeof(request_url));
            if (thread[i].running == 0)
            {
                thread[i].thread_index = i;
                thread[i].running = 1;
                pthread_create(&thread[i].tid, NULL, download_thread, &thread[i]);
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
    LOG(TAG, "Exit speedtest_download thread");
#endif

    return 0;
}

void *calculate_dl_speed_thread()
{
    double dl_speed = 0.0, duration = 0;
    while (1)
    {
        stop_dl_time = get_uptime();
        duration = stop_dl_time - start_dl_time;
        dl_speed = (double)total_dl_size / 1000 / 1000 / duration * 8;
        if (duration > 0)
        {
            printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\bDownload speed: %0.2lf Mbps", dl_speed);
            fflush(stdout);
        }
        usleep(500000);

        if (thread_all_stop)
        {
            stop_dl_time = get_uptime();
            duration = stop_dl_time - start_dl_time;
            dl_speed = (double)total_dl_size / 1000 / 1000 / duration * 8;
            if (duration > 0)
            {
                printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\bDownload speed: %0.2lf Mbps", dl_speed);
                printf("\n");
                fflush(stdout);
            }
            break;
        }
    }

    return NULL;
}

// void *https_download_thread(void *arg)
// {
//     const char *TAG = "HTTPS_DOWNLOAD_THREAD";
//     char data_log[200] = {0};

//     thread_t *t_arg = arg;
//     int i = t_arg->thread_index;

//     SSL_CTX *ctx;
//     SSL *ssl;
//     FILE *fp = NULL;
//     int fd;
//     char sbuf[256] = {0}, tmp_path[128] = {0};
//     int bytes_sent = 0;
//     int bytes_received = 0;
//     char rbuf[DL_BUFFER_SIZE] = {0};
//     int ret;

//     build_request(sbuf, thread[i].domain_name, thread[i].request_url);

//     ret = make_request(fd, sbuf);
//     error_unless(ret != -1, "Could not make connection to '%s'", thread[i].domain_name);

//     SSL_library_init();
//     ctx = InitCTX();
//     ssl = SSL_new(ctx); /* create new SSL connection state */

//     fd = init_connection(&(thread[i].servinfo));
//     error_unless(fd > 0, "Could not make connection to '%s'", thread[i].domain_name);

//     SSL_set_fd(ssl, fd); /* attach the socket descriptor */

//     ret = SSL_connect(ssl);
//     if (ret <= 0) /* perform the connection */
//     {
//         printf("Failed to set SSL file descriptor\n");
//         return -1;
//     }

//     build_request(sbuf, thread[i].domain_name, thread[i].request_url);

//     bytes_sent = SSL_write(ssl, sbuf, strlen(sbuf));
//     if (bytes_sent <= 0)
//     {
//         printf("Failed to send HTTPS request\n");
//         return -1;
//     }

//     while ((bytes_received = SSL_read(ssl, rbuf, sizeof(rbuf) - 1)) > 0)
//     {
//         rbuf[bytes_received] = '\0';

//         // Process the received data as needed

//         printf("%s", rbuf);
//     }

//     if (bytes_received < 0)
//     {
//         printf("Error while receiving server's response\n");
//         goto error;
//     }

// error:
//     if (fd)
//         close(fd);
//     thread[i].running = 0;
//     return NULL;
// }