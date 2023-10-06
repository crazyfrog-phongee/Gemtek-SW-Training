#ifndef LOG_USER_H_
#define LOG_USER_H_

void print_logged_users(int log_fd);
int open_log_file(const char *file_path);

#endif