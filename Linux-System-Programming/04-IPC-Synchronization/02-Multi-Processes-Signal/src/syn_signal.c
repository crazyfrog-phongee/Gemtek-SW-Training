#define _DEFAULT_SOURCE

#include <stdio.h>
#include <unistd.h> /* Need for functions such as fork() */
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

#include "syn_signal.h"

extern volatile int received_sig;

void print_wait_status(const char *msg, const int status)
{
    if (WIFEXITED(status))
    {
        printf("Normally termination, status = %d\n", WEXITSTATUS(status));
    }
    else if (WIFSIGNALED(status))
    {
        printf("killed by signal, value = %d (%s)\n", WTERMSIG(status), strsignal(WTERMSIG(status)));
    }
    else if (WCOREDUMP(status))
    {
        printf(" (core dumped)");
        printf("\n");
    }
    else if (WIFSTOPPED(status))
    {
        printf("child stopped by signal %d (%s)\n",
               WSTOPSIG(status), strsignal(WSTOPSIG(status)));
    }
    else if (WIFCONTINUED(status))
    {
        printf("child continued\n");
    }
    else
    { /* Should never happen */
        printf("what happened to this child? (status=%x)\n",
               (unsigned int)status);
    }

    return;
}

void signal_handler(int signum)
{
    if (signum == SIGUSR1)
    {
        received_sig = 1;
    }
    else if (signum == SIGCHLD)
    {
        /* Print the end status of the child process */
        int rv, status;
        
        rv = wait(&status);
        if (rv == -1)
        {
            printf("wait() unsucessful\n");
        }
        else
        {
            print_wait_status(NULL, status);
        }
    }
}