#include "shell_errors.h"
#include <stdio.h>

void print_error(char *error, char *arg){
    if (arg == NULL) arg = "error";
    printf("%s: ", arg);
    printf("%s\n", error);
}
