/*
 * speedtest_download.h
 *
 * Created on: Oct 10, 2023
 * Author: PhongEE
 */
#ifndef SPEEDTEST_DOWNLOAD_H_
#define SPEEDTEST_DOWNLOAD_H_

#include <sys/types.h>
#include <pthread.h>

#include "speedtest_server.h"
#include "main.h"

#define DL_BUFFER_SIZE 8192

int speedtest_download(server_data_t *nearest_server, thread_t *thr, pthread_mutex_t *mtx);
void *calculate_dl_speed_thread();
// int *https_download_thread(server_data_t *nearest_server, thread_t *thr, pthread_mutex_t *mtx);

#endif