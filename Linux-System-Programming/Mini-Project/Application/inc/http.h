#ifndef HTTP_H_
#define HTTP_H_
#include <netinet/in.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "main.h"

#define TIMEOUT_CONNECT 5

int get_ipv4_addr(char *domain_name, struct sockaddr_in *servinfo);
int init_connection(struct sockaddr_in *serv);
int build_http_get(char *sbuf, char *domain_name, char *request_url);
int build_http_post(char *sbuf, char *domain_name, char *request_url, unsigned long int content_length);
int make_request(int sockfd, char *sbuf);
int get_http_file(struct sockaddr_in *serv, char *domain_name, char *request_url, char *filename);
int get_ip_address_position(char *fileName, client_data_t *client_data);
SSL_CTX *InitCTX(void);
void ShowCerts(SSL *ssl);
void LoadCertificates(SSL_CTX *ctx, char *CertFile, char *KeyFile);
int get_https_file(struct sockaddr_in *serv, char *domain_name, char *request_url, char *filename);

#endif