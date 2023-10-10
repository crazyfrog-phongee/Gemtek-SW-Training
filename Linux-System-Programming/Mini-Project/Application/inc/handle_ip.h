#ifndef HANDLE_IP_H_
#define HANDLE_IP_H_
#include <netinet/in.h>

#include "main.h"

int get_ipv4_addr(char *domain_name, struct sockaddr_in *servinfo);
int init_connection(struct sockaddr_in *serv);
int build_request(char *sbuf, char *domain_name, char *request_url);
int make_request(int sockfd, char *sbuf);
int get_http_file(struct sockaddr_in *serv, char *domain_name, char *request_url, char *filename);
int get_ip_address_position(char *fileName, client_data_t *client_data);

#endif