#ifndef INPUT_USER_H_
#define INPUT_USER_H_

#include <stdbool.h>

#define MAX_INPUT_SIZE 255

bool get_input_from_user(char *str);
bool get_filepath_from_user(char *filepath);

#endif