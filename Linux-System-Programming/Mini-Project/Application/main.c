#include <stdio.h>
#include <string.h>

#include "handle_ip.h"
#include "common.h"
#include "main.h"
#include "speedtest_server.h"
#include "speedtest_download.h"

static thread_t thread[THREAD_NUMBER];
long int total_dl_size = 0, total_ul_size = 0;
static pthread_mutex_t total_dl_size_mutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char const *argv[])
{
    const char *TAG = "MAIN";
    struct sockaddr_in servinfo;
    client_data_t client_data;
    server_data_t best_server;

    pthread_t tid;

    memset(&client_data, 0, sizeof(client_data_t));
    memset(&best_server, 0, sizeof(server_data_t));

    strcpy(best_server.domain_name, "speedtestkv1dp1.viettel.vn");
    strcpy(best_server.url, "http://speedtestkv1dp1.viettel.vn/speedtest/upload.php");
    if (1 != get_ipv4_addr(best_server.domain_name, &(best_server.servinfo)))
    {
        return -1;
    }

    if (1 == get_ipv4_addr(SPEEDTEST_DOMAIN_NAME, &servinfo))
    {
        if (!get_http_file(&servinfo, SPEEDTEST_DOMAIN_NAME, CONFIG_REQUEST_URL, CONFIG_REQUEST_URL))
        {
            printf("Can't get your IP address information.\n");
            return 0;
        }
    }
    if (1 == get_ipv4_addr(SPEEDTEST_SERVERS_DOMAIN_NAME, &servinfo))
    {
        if (!get_http_file(&servinfo, SPEEDTEST_SERVERS_DOMAIN_NAME, SERVERS_LOCATION_REQUEST_URL, SERVERS_LOCATION_REQUEST_URL))
        {
            printf("Can't get servers list.\n");
            return 0;
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

    pthread_create(&tid, NULL, calculate_dl_speed_thread, NULL);
    speedtest_download(&best_server, thread, &total_dl_size_mutex);

    return 0;
}
