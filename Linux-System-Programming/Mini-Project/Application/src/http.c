#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "http.h"
#include "common.h"

static const char *TAG = "HTTP";

int get_ipv4_addr(char *domain_name, struct sockaddr_in *servinfo)
{
    struct addrinfo hints, *addrinfo, *p;
    int status;
    int len = strlen(domain_name);
    char token[len];
    int is_used_token = 1;

    if (is_used_token)
    {
        memset(token, 0, len);
        if (strstr(domain_name, ":8080") != NULL)
        {

            strncpy(token, domain_name, (len - 5));
            token[len - 5] = '\0';
        }
        else
        {
            strcpy(token, domain_name);
        }
    }
    else
    {
        strcpy(token, domain_name);
    }

#if (DEBUG_LVL > 0)
    LOG(TAG, token);
#endif

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

#if (USE_HTTPS)
    status = getaddrinfo(token, "https", &hints, &addrinfo);
#else
    status = getaddrinfo(token, "http", &hints, &addrinfo);
#endif

    if (status != 0)
    {
#if (DEBUG_LVL > 0)
        printf("Error num: %d\n", status);
        printf("Error def: %s\n", gai_strerror(status));
        perror("getaddrinfo");
#endif
        printf("Resolve DNS Failed: Can't get ip address! (%s)\n", token);
        return 0;
    }

    for (p = addrinfo; p != NULL; p = p->ai_next)
    {
        if (p->ai_family == AF_INET)
        {
            memcpy(servinfo, (struct sockaddr_in *)p->ai_addr, sizeof(struct sockaddr_in));
        }
    }
    freeaddrinfo(addrinfo);

    return 1;
}

int init_connection(struct sockaddr_in *serv)
{
    int sockfd;
    int status;
    struct timeval timeout;

    sockfd = socket(serv->sin_family, SOCK_STREAM, 0);
    jump_unless(sockfd > 0);


    timeout.tv_sec = TIMEOUT_CONNECT;
    timeout.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    // if (connect(sockfd, (struct sockaddr *)serv, sizeof(struct sockaddr)) == -1)
    // {
    //     if (errno != EINPROGRESS)
    //     {
    //         // printf("\n\nCreate connection to %s failed!\n", domain_name);
    //         perror("Error: ");
    //         if (sockfd)
    //             close(sockfd);
    //         return 0;
    //     }
    // }
    status = connect(sockfd, (struct sockaddr *)serv, sizeof(struct sockaddr));
    jump_unless(status == 0);
    
    return sockfd;

error:
    return -1;
}

int build_http_get(char *sbuf, char *domain_name, char *request_url)
{
    sprintf(sbuf,
            "GET /%s HTTP/1.0\r\n"
            "Host: %s\r\n"
            "User-Agent: status\r\n"
            "Accept: */*\r\n\r\n",
            request_url, domain_name);

    return 0;
}

int build_http_post(char *sbuf, char *domain_name, char *request_url, unsigned long int content_length)
{
    sprintf(sbuf,
            "POST /%s HTTP/1.0\r\n"
            "Content-type: application/x-www-form-urlencoded\r\n"
            "Host: %s\r\n"
            "Content-Length: %ld\r\n\r\n",
            request_url, domain_name, content_length);

    return 0;
}

int make_request(int sockfd, char *sbuf)
{
    if (send(sockfd, sbuf, strlen(sbuf), 0) != strlen(sbuf))
    {
        perror("Can't send data to server\n");
        return -1;
    }

    return 0;
}

int get_http_file(struct sockaddr_in *serv, char *domain_name, char *request_url, char *filename)
{
    char sbuf[256] = {0}, tmp_path[128] = {0};
    char rbuf[8192];
    struct timeval tv;
    fd_set fdSet;
    FILE *fp = NULL;
    int fd;
    int ret;

    fd = init_connection(serv);
    printf("HTTP: %d\n", fd);
    error_unless(fd > 0, "Could not make connection to '%s'", domain_name);

    build_http_get(sbuf, domain_name, request_url);

    ret = make_request(fd, sbuf);
    error_unless(ret != -1, "Could not make connection to '%s'", domain_name);

    sprintf(tmp_path, "%s%s", FILE_DIRECTORY_PATH, filename);
    fp = fopen(tmp_path, "w+");

    while (1)
    {
        char *ptr = NULL;
        memset(rbuf, 0, sizeof(rbuf));
        FD_ZERO(&fdSet);
        FD_SET(fd, &fdSet);

        tv.tv_sec = 3;
        tv.tv_usec = 0;
        LOG(TAG, "Get timeout");
        int status = select(fd + 1, &fdSet, NULL, NULL, &tv);
        int i = recv(fd, rbuf, sizeof(rbuf), 0);
        if (status > 0 && FD_ISSET(fd, &fdSet))
        {
            if (i < 0)
            {
                printf("Can't get http file!\n");
                close(fd);
                fclose(fp);
                return 0;
            }
            else if (i == 0)
            {
                break;
            }
            else
            {
                if ((ptr = strstr(rbuf, "\r\n\r\n")) != NULL)
                {
                    ptr += 4;
                    fwrite(ptr, 1, strlen(ptr), fp);
                }
                else
                {
                    fwrite(rbuf, 1, i, fp);
                }
            }
        }
    }

    close(fd);
    fclose(fp);
    return 1;

error:
    if (fd > 0)
    {
        close(fd);
    }
    return -1;
}

int get_ip_address_position(char *fileName, client_data_t *client_data)
{
    FILE *fp = NULL;
    char filePath[128] = {0}, line[512] = {0}, lat[64] = {0}, lon[64] = {0};
    sprintf(filePath, "%s%s", FILE_DIRECTORY_PATH, fileName);

    if ((fp = fopen(filePath, "r")) != NULL)
    {
        while (fgets(line, sizeof(line) - 1, fp) != NULL)
        {
            if (!strncmp(line, "<client", 7))
            {
                // ex: <client ip="61.216.30.97" lat="24.7737" lon="120.9436" isp="HiNet" isprating="3.2" rating="0" ispdlavg="50329" ispulavg="22483" loggedin="0"/>
                sscanf(line, "%*[^\"]\"%255[^\"]\"%*[^\"]\"%255[^\"]\"%*[^\"]\"%255[^\"]\"%*[^\"]\"%255[^\"]\"", client_data->ipAddr, lat, lon, client_data->isp);
                client_data->latitude = atof(lat);
                client_data->longitude = atof(lon);
                break;
            }
        }
        fclose(fp);
    }
    return 1;
}

SSL_CTX *InitCTX(void)
{
    SSL_METHOD *method;
    SSL_CTX *ctx;
    OpenSSL_add_all_algorithms();     /* Load cryptos, et.al. */
    SSL_load_error_strings();         /* Bring in and register error messages */
    method = TLSv1_2_client_method(); /* Create new client-method instance */
    ctx = SSL_CTX_new(method);        /* Create new context */
    if (ctx == NULL)
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    return ctx;
}

void ShowCerts(SSL *ssl)
{
    X509 *cert;
    char *line;
    cert = SSL_get_peer_certificate(ssl); /* get the server's certificate */
    if (cert != NULL)
    {
        printf("Server certificates:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("Subject: %s\n", line);
        free(line); /* free the malloc'ed string */
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("Issuer: %s\n", line);
        free(line);      /* free the malloc'ed string */
        X509_free(cert); /* free the malloc'ed certificate copy */
    }
    else
        printf("Info: No client certificates configured.\n");
}

int get_https_file(struct sockaddr_in *serv, char *domain_name, char *request_url, char *filename)
{
    SSL_CTX *ctx;
    SSL *ssl;
    FILE *fp = NULL;
    int fd;
    char sbuf[256] = {0}, tmp_path[128] = {0};
    int bytes_sent = 0;
    int bytes_received = 0;
    char rbuf[8192] = {0};
    int ret;

    struct timeval tv;
    fd_set fdSet;

    SSL_library_init();
    ctx = InitCTX();
    ssl = SSL_new(ctx); /* create new SSL connection state */

    fd = init_connection(serv);

    SSL_set_fd(ssl, fd); /* attach the socket descriptor */

    ret = SSL_connect(ssl);
    if (ret <= 0) /* perform the connection */
    {
        printf("Failed to set SSL file descriptor\n");
        return -1;
    }

    build_http_get(sbuf, domain_name, request_url);

    bytes_sent = SSL_write(ssl, sbuf, strlen(sbuf));
    if (bytes_sent <= 0)
    {
        printf("Failed to send HTTPS request\n");
        return -1;
    }

    sprintf(tmp_path, "%s%s", FILE_DIRECTORY_PATH, filename);
    fp = fopen(tmp_path, "w+");

    while (1)
    {
        char *ptr = NULL;
        memset(rbuf, 0, sizeof(rbuf));
        FD_ZERO(&fdSet);
        FD_SET(fd, &fdSet);

        tv.tv_sec = 3;
        tv.tv_usec = 0;
        int status = select(fd + 1, &fdSet, NULL, NULL, &tv);
        int i = SSL_read(ssl, rbuf, sizeof(rbuf));
        if (status > 0 && FD_ISSET(fd, &fdSet))
        {
            if (i < 0)
            {
                printf("Can't get http file!\n");
                close(fd);
                fclose(fp);
                return -1;
            }
            else if (i == 0)
            {
                break;
            }
            else
            {
                if ((ptr = strstr(rbuf, "\r\n\r\n")) != NULL)
                {
                    ptr += 4;
                    fwrite(ptr, 1, strlen(ptr), fp);
                }
                else
                {
                    fwrite(rbuf, 1, i, fp);
                }
            }
        }
    }

    close(fd); /* close socket */
    SSL_free(ssl);
    SSL_CTX_free(ctx); /* release context */

    return 0;
}