#include <stdio.h>
#include <string.h>

#include "handle_time.h"

float get_uptime(void)
{
    FILE *fp;
    float uptime, idle_time;

    if ((fp = fopen("/proc/uptime", "r")) != NULL)
    {
        fscanf(fp, "%f %f\n", &uptime, &idle_time);
        fclose(fp);
        return uptime;
    }
    return -1;
}

void set_interval_timer(int which, int duration)
{
    struct itimerval timerVal;

    memset(&timerVal, 0, sizeof(struct itimerval));
    timerVal.it_value.tv_sec = SPEEDTEST_DURATION;
    timerVal.it_value.tv_usec = 0;
    setitimer(ITIMER_REAL, &timerVal, NULL);

    return;
}
