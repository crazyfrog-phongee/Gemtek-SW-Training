#define _DEFAULT_SOURCE /* Define for getting the type values because the d_type field is non-standard*/
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdio_ext.h>
#include <unistd.h>
#include <stdlib.h>

#include "file_handling.h"

void get_type_of_file(unsigned char d_type, char *type_name)
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
}

bool check_empty_dir(const char file_name[])
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

int check_file_exist(const char *file_name)
{
    FILE *file = NULL;

    if (file_name == NULL)
    {
        return 0;
    }

    if (!check_empty_dir(file_name))
    {
        return 0;
    }

    if ((file = fopen(file_name, "r")))
    {
        fclose(file);
        return 1;
    }
    return 0;
}

bool get_file_name(char *file_name)
{
    char pre_file_name[MAX_FILE_SIZE] = "";
    strcpy(pre_file_name, file_name);

    /* Get file name */
    printf("Enter file name (or e to exit): ");
    get_input_from_user(file_name);
    if (strcmp(file_name, "e") == 0)
    {
        strcpy(file_name, pre_file_name);
        return false;
    }

    /* Check if file exist */
    while (!check_file_exist(file_name))
    {
        printf("File not exist, enter again (or e to exit): ");
        get_input_from_user(file_name);
        if (strcmp(file_name, "e") == 0)
        {
            strcpy(file_name, pre_file_name);
            return false;
        }
    }

    return true;
}

void write_to_file(char *dir, char *text)
{
    FILE *fp = NULL;
    
    fp = fopen(dir, "w");
    fprintf(fp, "%s", text);
    fclose(fp);
}