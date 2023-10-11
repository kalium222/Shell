#ifndef SHELL_ERRORS_H_
#define SHELL_ERRORS_H_

#define COMMAND_NOT_FOUND "command not found"
#define NO_SUCH_FILE_OR_DIRECTORY "No such file or directory"
#define PERMISSION_DENIED "Permission denied"
#define DUPLICATED_INPUT_REDIRECTION "duplicated input redirection"
#define DUPLICATED_OUTPUT_REDIRECTION "duplicated output redirection"
#define SYXTAX_ERROR "syntax error near unexpected token"
#define MISSING_PROGRAM "missing program"

void print_error(char *error, char *arg);

#endif
