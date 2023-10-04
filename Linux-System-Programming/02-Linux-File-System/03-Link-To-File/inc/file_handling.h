#ifndef _FILE_HANDLING_H_
#define _FILE_HANDLING_H_

#include <stdbool.h>

#define MAX_FILE_SIZE 255

typedef enum file_status
{
    ERROR_NONE,
    FILE_NOT_EXIST_IN_DIR = 1,
    FILE_EXIST_IN_DIR,
    ERROR_OPEN_DIRECTORY,
    ERROR_CLOSE_DIRECTORY,
} error_file_status_t;

error_file_status_t search_file_in_directory(const char *dir, const char *file_name, int depth, char *ret_dir);
void create_hard_link_file(const char *source_file, const char *destination_file);
void create_soft_link_file(const char *source_file, const char *destination_file);
bool check_empty_dir(const char *file_name);
bool is_valid_file_name(const char *file_name);

#endif