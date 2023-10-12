#define _GNU_SOURCE /* Be careful if using signal(). Notice at signal mask */
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include "main.h"
#include "common.h"
#include "http.h"
#include "handle_time.h"

#include "speedtest_server.h"
#include "speedtest_download.h"
#include "speedtest_upload.h"

static thread_t thread[THREAD_NUMBER];
long int total_dl_size = 0, total_ul_size = 0;
static pthread_mutex_t total_dl_size_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t total_ul_size_mutex = PTHREAD_MUTEX_INITIALIZER;
int thread_all_stop = 0;

void stop_all_thread(int signo)
{
    if (signo == SIGALRM)
    {
        thread_all_stop = 1;
    }

    return;
}

int main(int argc, char const *argv[])
{
    const char *TAG = "MAIN";
    struct sockaddr_in servinfo;
    client_data_t client_data;
    server_data_t nearest_servers[NEAREST_SERVERS_NUM];
    server_data_t best_server;
    server_data_t best_server_https;
    int best_server_index;

    pthread_t tid;

    for (int i = 0; i < NEAREST_SERVERS_NUM; i++)
    {
        memset(&nearest_servers[i], 0, sizeof(server_data_t));
    }
    memset(&client_data, 0, sizeof(client_data_t));
    memset(&best_server, 0, sizeof(server_data_t));

    strcpy(best_server.domain_name, "speedtestkv1dp1.viettel.vn");
    strcpy(best_server.url, "http://speedtestkv1dp1.viettel.vn/speedtest/upload.php");

    // strcpy(best_server_https.domain_name, "www.speedtest.net");
    // strcpy(best_server_https.url, "https://www.speedtest.net/speedtest-config.php");

    if (1 != get_ipv4_addr(best_server.domain_name, &(best_server.servinfo)))
    {
        return -1;
    }

    if (1 == get_ipv4_addr(SPEEDTEST_DOMAIN_NAME, &servinfo))
    {
        if (!get_http_file(&servinfo, SPEEDTEST_DOMAIN_NAME, CONFIG_REQUEST_URL, CONFIG_REQUEST_URL))
        {
            printf("Can't get your IP address information.\n");
            return -1;
        }
    }
    if (1 == get_ipv4_addr(SPEEDTEST_SERVERS_DOMAIN_NAME, &servinfo))
    {
        if (!get_http_file(&servinfo, SPEEDTEST_SERVERS_DOMAIN_NAME, SERVERS_LOCATION_REQUEST_URL, SERVERS_LOCATION_REQUEST_URL))
        {
            printf("Can't get servers list.\n");
            return -1;
        }
    }

    get_ip_address_position(CONFIG_REQUEST_URL, &client_data);

#if (DEBUG_LVL > 0)
    printf("============================================\n");
    printf("Your IP Address : %s\n", client_data.ipAddr);
    printf("Your IP Location: %0.4lf, %0.4lf\n", client_data.latitude, client_data.longitude);
    printf("Your ISP        : %s\n", client_data.isp);
    printf("============================================\n");
#endif

    if (get_nearest_server(client_data.latitude, client_data.longitude, nearest_servers) == 0)
    {
        printf("Can't get server list.\n");
        return 0;
    }

    best_server_index = get_best_server(nearest_servers);
    printf("best_server_index: %d\n", best_server_index);
    if (best_server_index < 0 || best_server_index > NEAREST_SERVERS_NUM)
    {
        printf("Can't get the best server.\n");
        return -1;
    }
    
#if (DEBUG_LVL > 0)
    get_best_server_info(nearest_servers, best_server_index);
#endif

    /* Set speedtest timer */
    signal(SIGALRM, stop_all_thread);
    set_interval_timer(ITIMER_REAL, NULL);

    /* Downloading task */
    pthread_create(&tid, NULL, calculate_dl_speed_thread, NULL);
    speedtest_download(&nearest_servers[best_server_index], thread, &total_dl_size_mutex);
    pthread_join(tid, NULL);

    /* Prepare for uploading task */
    sleep(2);
    thread_all_stop = 0;

    set_interval_timer(ITIMER_REAL, NULL);
    pthread_create(&tid, NULL, calculate_ul_speed_thread, NULL);
    speedtest_upload(&nearest_servers[best_server_index], thread, &total_ul_size_mutex);
    pthread_join(tid, NULL);

    return 0;
}
