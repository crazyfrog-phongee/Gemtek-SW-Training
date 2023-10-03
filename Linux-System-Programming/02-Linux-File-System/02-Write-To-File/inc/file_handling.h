#ifndef _FILE_HANDLING_H_
#define _FILE_HANDLING_H_

#include <stdbool.h>

#define MAX_FILE_SIZE 255
#define MAX_LINE_SIZE 256
#define MAX_DIRECTORY_NAME 256
#define BUFFER_LEN    4026

/**
 * @brief Writes a string to a file.
 *
 * This function writes the contents of the `text` parameter to the file specified by the `dir` parameter.
 *
 * @param dir The directory path of the file to write.
 * @param text The string to write to the file.
 */
void write_to_file(char *dir, char *text);

/**
 * @brief Checks if a directory is empty.
 *
 * This function checks if the directory specified by the `file_name` parameter is empty or not.
 *
 * @param file_name The name of the directory to check.
 * @return True if the directory is empty, false otherwise.
 */
bool check_empty_dir(const char file_name[]);

/**
 * @brief Checks if a file exists.
 *
 * This function checks if the file specified by the `file_name` parameter exists or not.
 *
 * @param file_name The name of the file to check.
 * @return 0 if the file exists, -1 otherwise.
 */
int check_file_exist(const char file_name[]);

void get_input_from_user(char *dir_name);
void get_type_of_file(unsigned char d_type, char *type_name);
bool get_file_name(char *file_name);

#endif