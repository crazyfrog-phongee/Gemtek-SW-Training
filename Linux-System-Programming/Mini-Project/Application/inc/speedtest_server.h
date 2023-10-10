/*
 * speedtest_server.h
 *
 * Created on: Oct 10, 2023
 * Author: PhongEE
 */
#ifndef SPEEDTEST_SERVER_H_
#define SPEEDTEST_SERVER_H_

#include <netinet/in.h>

typedef struct server_data
{
    char url[128];
    double latitude;
    double longitude;
    char name[128];
    char country[128];
    double distance;
    int latency;
    char domain_name[128];
    struct sockaddr_in servinfo;
} server_data_t;

#endif