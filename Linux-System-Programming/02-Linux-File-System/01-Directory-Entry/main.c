#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
    DIR *dir;
    struct dirent *entry;
    int ret;

    dir = opendir(".");
    if (NULL == dir)
    {
        printf("Error opening directory.\n");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL)
    {
        printf("%s %d\n", entry->d_name, entry->d_type);
    }

    ret = closedir(dir);
    if (ret == -1)
    {
        printf("Error closing directory.\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}
