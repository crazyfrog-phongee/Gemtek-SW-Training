#define _DEFAULT_SOURCE
#include <stdio.h>
#include <unistd.h> /* Need for functions such as fork() */
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

#include "file_handle.h"
#include "common.h"
#include "syn_signal.h"

const char *file_name = "output.txt";
volatile int received_sig = 0;

int get_last_line_number(const char *filename)
{
    int last_line_number = 0;
    char last_line[MAX_LINE_LENGTH] = {0};

    file_handle_get_last_line(filename, last_line);
    sscanf(last_line, "Process %*d: %d", &last_line_number);

    return last_line_number;
}

void child_process_task(pid_t parent_pid, int counter)
{
    const char *TAG = "CHILD";
    char msg[MAX_LINE_LENGTH] = {0};
    while (1)
    {
        pause();
        if (received_sig == 1)
        {
            counter = get_last_line_number(file_name);

            /* Block code for break while loop */
            if (counter == 15)
            {
                break;
            }

            counter++;
            sprintf(msg, "Process %i: %i\n", getpid(), counter);
            file_handle_write(file_name, msg);
            LOG(TAG, msg);
            kill(getppid(), SIGUSR1);
            received_sig = 0;
        }
    }

    printf("Child process exits\n");
}

void parent_process_task(pid_t child_pid, int counter)
{
    const char *TAG = "PARENT";
    char msg[MAX_LINE_LENGTH] = {0};

    counter = get_last_line_number(file_name);
    counter++;
    sprintf(msg, "Process %i: %i\n", getpid(), counter);
    file_handle_write(file_name, msg);

    kill(child_pid, SIGUSR1);

    while (1)
    {
        if (received_sig == 1)
        {
            counter = get_last_line_number(file_name);
            counter++;
            sprintf(msg, "Process %i: %i\n", getpid(), counter);
            file_handle_write(file_name, msg);
            LOG(TAG, msg);
            kill(child_pid, SIGUSR1);
            received_sig = 0;
        }
        pause();
    }
}

int main(int argc, char const *argv[])
{

    int counter = 0;

    signal(SIGUSR1, signal_handler);

    pid_t parent_pid, child_pid;
    parent_pid = getpid();
    switch (child_pid = fork())
    {
    case -1:
        /* Handle error */
        break;

    case 0:
        /* Perform actions specific to child */
        child_process_task(parent_pid, counter);
        break;

    default:
        sleep(1); /* Sleep for initialize child process */
        parent_process_task(child_pid, counter);
        break;
        /* Perform actions specific to parent */
    }

    /* Both parent and child come here */

    return 0;
}