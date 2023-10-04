#include <stdio.h>
#include <string.h>
#include <stdio_ext.h>

#include "input_user.h"
#include "file_handling.h"

bool get_input_from_user(char *str)
{
    __fpurge(stdin);
    fgets(str, MAX_INPUT_SIZE, stdin);
    str[strcspn(str, "\n")] = '\0';

    if (strcmp(str, "!exit") == 0)
    {
        return false;
    }

    return true;
}

bool get_filepath_from_user(char *filepath)
{
    get_input_from_user(filepath);

    if (!check_empty_dir(filepath))
    {
        return false;
    }

    if (!is_valid_file_name(filepath))
    {
        return false;
    };

    return true;
}