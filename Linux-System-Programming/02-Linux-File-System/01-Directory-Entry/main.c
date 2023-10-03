#include "file_handling.h"

int main(int argc, char const *argv[])
{
    char dir_name[MAX_DIRECTORY_NAME] = {0};

    /* Get directory name from the user */
    get_input_from_user(dir_name);

    /* Print all file names and their types from the directory */
    print_from_directory(dir_name);

    return 0;
}
