#ifndef MAIN_H_
#define MAIN_H_

#include <pthread.h>

#define SPEEDTEST_DOMAIN_NAME "www.speedtest.net"
#define SPEEDTEST_SERVERS_DOMAIN_NAME "c.speedtest.net"

#define CONFIG_REQUEST_URL "speedtest-config.php"
#define SERVERS_LOCATION_REQUEST_URL "speedtest-servers-static.php?"

#define FILE_DIRECTORY_PATH "/tmp/"

#define THREAD_NUMBER 3

typedef struct client_data
{
    char ipAddr[128];
    double latitude;
    double longitude;
    char isp[128];
} client_data_t;

typedef struct thread
{
    int thread_index;
    int running;
    pthread_t tid;
    char domain_name[128];
    char request_url[128];
    struct sockaddr_in servinfo;
} thread_t;

#endif