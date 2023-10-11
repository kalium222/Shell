#include "built-in.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

const struct built_in built_in_list[] = {
    {"exit", 0, built_in_exit},
    {"cd", 0, built_in_cd},
    {"pwd", 1, built_in_pwd},
    {NULL, 0, NULL}
};

int built_in_exit(int argc, char **argv) {
    for (int i=0; i<argc; i++) {
        printf("%s ", argv[i]);
    }
    printf("\n");
    exit(0);
    return 1;
}

int built_in_cd(int argc, char **argv) {
    switch (argc)
    {
    case 1:
        if (chdir(getenv("HOME")) == -1) {
            perror("cd");
        }
        break;
    case 2:
        if (chdir(argv[1]) == -1) {
            perror("cd");
        }
        break;
    default:
        perror("cd: too many arguments");
        break;
    }
    return 0;
}

int built_in_pwd(int argc, char **argv) {
    if (argc > 1) {
        perror("pwd: too many arguments");
        return 0;
    }
    if (argv[0]==NULL) {
        perror("pwd: invalid argument");
        return 0;
    }
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("pwd");
    } else {
        printf("%s\n", cwd);
    }
    return 0;
}

int is_built_in(char *cmd) {
    for (int i=0; built_in_list[i].name != NULL; i++) {
        if (strcmp(cmd, built_in_list[i].name) == 0) {
            return 1;
        }
    }
    return 0;
}

int is_run_in_child(char *cmd) {
    for (int i=0; built_in_list[i].name != NULL; i++) {
        if (strcmp(cmd, built_in_list[i].name) == 0) {
            return built_in_list[i].in_child_proc;
        }
    }
    perror("is_run_in_child: command not found");
    return 0;
}

int execute_built_in(char *cmd, int argc, char **argv) {
    for (int i=0; built_in_list[i].name != NULL; i++) {
        if (strcmp(cmd, built_in_list[i].name) == 0) {
            return built_in_list[i].func(argc, argv);
        }
    }
    perror("execute_built_in: command not found");
    return 0;
}
