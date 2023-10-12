/*
 * handle_time.h
 *
 * Created on: Oct 11, 2023
 * Author: PhongEE
 */
#ifndef HANDLE_TIME_H_
#define HANDLE_TIME_H_

#include <sys/time.h>

#define SPEEDTEST_DURATION 10

float get_uptime(void);
void set_interval_timer(int which, int duration);

#endif