#include <stdio.h>
#include <stdlib.h>

#include "file_handling.h"

int main(int argc, char const *argv[])
{
    char filename[MAX_FILE_SIZE] = "";
    char text[BUFFER_LEN] = "";

    if (!get_file_name(filename))
    {
        exit(EXIT_FAILURE);
    }

    printf("Enter text to write to file: ");
    get_input_from_user(text);
    write_to_file(filename, text);
    printf("Write '%s' to file '%s' successfully\n", text, filename);

    return 0;
}
