#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <syslog.h>

#include "become_daemon.h"
#include "log_user.h"

volatile int received_sigterm = 0;

void sigterm_handler(int signum)
{
    if (signum == SIGTERM)
    {  
        /* Synchronization Technical */
        received_sigterm = 1;
    }
}

int main(int argc, char const *argv[])
{
    int ret;
    int log_fd = -1;

    ret = become_daemon(0);
    if (ret)
    {
        syslog(LOG_USER | LOG_ERR, "error starting");
        closelog();
        return EXIT_FAILURE;
    }

    signal(SIGTERM, sigterm_handler);

    log_fd = open_log_file("/var/log/login_daemon.log");
    if (log_fd == -1)
    {
        syslog(LOG_USER | LOG_ERR, "error in opening file");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        if (received_sigterm == 1)
        {
            break;
        }

        print_logged_users(log_fd);

        sleep(60);
    }

    close(log_fd);

    return 0;
}
