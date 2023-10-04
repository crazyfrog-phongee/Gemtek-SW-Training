#define _DEFAULT_SOURCE /* Define for getting the type values because the d_type field is non-standard*/
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>

#include "file_handling.h"

error_file_status_t search_file_in_directory(const char *dir, const char *file_name, int depth, char *ret_dir)
{
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    // char ret_str[MAX_DIRECTORY_NAME] = {0};

    if ((dp = opendir(dir)) == NULL)
    {
        fprintf(stderr, "Cannot open directory: %s\n", dir);
        return ERROR_OPEN_DIRECTORY;
    }

    chdir(dir);
    while ((entry = readdir(dp)) != NULL)
    {
        // sprintf(ret_str, "%s/%s", ret_str, dir);
        lstat(entry->d_name, &statbuf);
        if (S_ISDIR(statbuf.st_mode))
        {
            /* Found a directory, but ignore . and .. */
            if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
            {
                continue;
            }

            // printf("%*s%s/\n", depth, "", entry->d_name);

            /* Recurse at a new indent level */
            search_file_in_directory(entry->d_name, file_name, depth + 4, ret_dir);
        }
        else
        {
            // printf("%*s%s\n", depth, "", entry->d_name);
            if (strcmp(entry->d_name, file_name) == 0) /* If file existed in directory */
            {
                sprintf(ret_dir, "%s/%s", dir, file_name);
                printf("%s\n", ret_dir);
                chdir("..");
                closedir(dp);
                return FILE_EXIST_IN_DIR;
            }
        }
    }

    chdir("..");
    closedir(dp);

    return FILE_NOT_EXIST_IN_DIR;
}

void create_hard_link_file(const char *source_file, const char *destination_file)
{
    /* Create the hard link */
    if (link(source_file, destination_file) != 0)
    {
        perror("Error creating hard link");
        return;
    }

    printf("Hard link created successfully.\n");

    return;
}

void create_soft_link_file(const char *source_file, const char *destination_file)
{
    /* Create the soft link */
    if (link(source_file, destination_file) != 0)
    {
        perror("Error creating soft link");
        return;
    }

    printf("Soft link created successfully.\n");

    return;
}

bool is_valid_file_name(const char *file_name)
{
    const char *invalid_chars = "\\/:*?\"<>|";

    /* Check if the file name is empty or exceeds the maximum length */
    if (strlen(file_name) == 0 || strlen(file_name) > MAX_FILE_SIZE)
    {
        return false;
    }

    /* Check if the file name contains any invalid characters */
    for (int i = 0; i < strlen(invalid_chars); i++)
    {
        if (strchr(file_name, invalid_chars[i]) != NULL)
        {
            return false;
        }
    }

    /* Check if the file name starts or ends with a space or a period */
    if (file_name[0] == ' ' || file_name[0] == '.' ||
        file_name[strlen(file_name) - 1] == ' ' || file_name[strlen(file_name) - 1] == '.')
    {
        return false;
    }

    return true;
}

bool check_empty_dir(const char *file_name)
{
    for (int index = 0; index < strlen(file_name); index++)
    {
        if (file_name[index] != '\\' && file_name[index] != '/' && file_name[index] != '.')
        {
            return true;
        }
    }

    return false;
}