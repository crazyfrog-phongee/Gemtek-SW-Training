#include <stdio.h>

#include "file_handling.h"
#include "input_user.h"

const char *destination_hard_link_file = "hard_link.txt";
const char *destination_soft_link_file = "soft_link.txt";

int main(int argc, char const *argv[])
{
    char ret_dir[255] = {0};
    char dir[255] = {0};

    get_filepath_from_user(dir);
    search_file_in_directory(dir, "hello_world.txt", 0, ret_dir);
    create_hard_link_file(ret_dir, destination_hard_link_file);
    create_soft_link_file(ret_dir, destination_soft_link_file);
    
    return 0;
}
