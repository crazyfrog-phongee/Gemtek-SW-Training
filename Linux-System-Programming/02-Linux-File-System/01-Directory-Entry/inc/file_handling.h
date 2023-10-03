#ifndef _FILE_HANDLING_H_
#define _FILE_HANDLING_H_

#define MAX_DIRECTORY_NAME   100
#define MAX_TYPE_NAME   30

typedef enum error_directory
{
    ERROR_NONE = 0,
    ERROR_INVALID_DIRECTORY,
    ERROR_OPEN_DIRECTORY,
    ERROR_CLOSE_DIRECTORY,
} error_directory_t;

void get_input_from_user(char *dir_name);
error_directory_t print_from_directory(char *dir_name);

#endif