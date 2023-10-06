#define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <pwd.h>
#include <time.h>
#include <utmp.h>
#include <stdlib.h>

int open_log_file(const char *file_path)
{
    int log_fd = 0;

    log_fd = open(file_path, O_CREAT | O_WRONLY | O_APPEND, 0644);

    return log_fd;
}
void print_logged_users(int log_fd)
{
    struct utmp *ut;

    setutent();
    while ((ut = getutent()) != NULL)
    {
        if (ut->ut_type == USER_PROCESS)
        {
            struct passwd *pw = getpwnam(ut->ut_user);
            if (pw != NULL)
            {
                time_t now = time(NULL);
                char *timestamp = ctime(&now);
                timestamp[strlen(timestamp) - 1] = '\0'; // Remove newline character
                dprintf(log_fd, "%s: User %s logged in\n", timestamp, pw->pw_name);
            }
        }
    }
    endutent();
}