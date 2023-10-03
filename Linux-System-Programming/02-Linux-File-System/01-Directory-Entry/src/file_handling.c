#define _DEFAULT_SOURCE /* Define for getting the type values because the d_type field is non-standard*/
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdio_ext.h>

#include "file_handling.h"

static void get_type_of_file(const unsigned char d_type, char *type_name)
{
    memset(type_name, 0, strlen(type_name));

    switch (d_type)
    {
    case DT_UNKNOWN:
        strcpy(type_name, "Unknown Type");
        break;
    case DT_REG:
        strcpy(type_name, "Regular File");
        break;
    case DT_DIR:
        strcpy(type_name, "Directory");
        break;
    case DT_SOCK:
        strcpy(type_name, "Local-domain Socket");
        break;
    case DT_CHR:
        strcpy(type_name, "Character Device");
        break;
    case DT_BLK:
        strcpy(type_name, "Block Device");
        break;
    case DT_LNK:
        strcpy(type_name, "Symbolic Link");
        break;
    default:
        break;
    }

    return;
}

void get_input_from_user(char *dir_name)
{
    __fpurge(stdin);
    fgets(dir_name, MAX_DIRECTORY_NAME, stdin);
    dir_name[strcspn(dir_name, "\n")] = '\0';

    return;
}

error_directory_t print_from_directory(char *dir_name)
{
    DIR *dir;
    char type_name[MAX_TYPE_NAME] = {0};
    struct dirent *entry;
    int ret = 0;

    dir = opendir(dir_name);
    if (NULL == dir)
    {
        return ERROR_OPEN_DIRECTORY;
    }

    /* Traversal Directory */
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }
        
        get_type_of_file(entry->d_type, type_name);
        printf("%s: %s\n", entry->d_name, type_name);
    }

    ret = closedir(dir);
    if (ret == -1)
    {
        return ERROR_CLOSE_DIRECTORY;
    }

    return ERROR_NONE;
}