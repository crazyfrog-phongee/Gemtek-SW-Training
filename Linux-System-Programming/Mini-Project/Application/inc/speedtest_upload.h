/*
 * speedtest_upload.h
 *
 * Created on: Oct 10, 2023
 * Author: PhongEE
 */
#ifndef SPEEDTEST_UPLOAD_H_
#define SPEEDTEST_UPLOAD_H_

#include "main.h"
#include "speedtest_server.h"

#define UL_BUFFER_SIZE 8192
#define UL_BUFFER_TIMES 10240

void *calculate_ul_speed_thread();
void *upload_thread(void *arg);
int speedtest_upload(server_data_t *nearest_server, thread_t *thr, pthread_mutex_t *mtx);

#endif