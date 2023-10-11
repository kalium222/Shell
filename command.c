#include "command.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "shell_errors.h"
// struct command* new_command(void) {
//     struct command* cmd = (struct command*)malloc(sizeof(struct command));
//     init_command(cmd);
//     return cmd;
// }

void init_command(struct command* cmd) {
    cmd->argc = 0;
    cmd->bg = 0;
    cmd->in_fd = STDIN_FILENO;
    cmd->out_fd = STDOUT_FILENO;
    cmd->append = 0;
}

void init_commands(struct command *commands, int cnt) {
    for (int i = 0; i < cnt; i++) {
        init_command(&commands[i]);
    }
}

struct command* new_commands(int max_cnt) {
    struct command* cmds = (struct command*)malloc(sizeof(struct command) * (size_t)max_cnt);
    if (cmds == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    init_commands(cmds, max_cnt);
    return cmds;
}

// void free_command(struct command* cmd) {
//     free(cmd);
// }

void free_commands(struct command* commands) {
    free(commands);
}

int set_in_fd(struct command* cmd, char* file_name) {
    if (cmd->in_fd != STDIN_FILENO) {
        print_error(DUPLICATED_INPUT_REDIRECTION, NULL);
        return 0;
    }
    cmd->in_fd = open(file_name, O_RDONLY);
    if (cmd->in_fd == -1) {
        print_error(NO_SUCH_FILE_OR_DIRECTORY, file_name);
        return 0;
    }
    return 1;
}

int set_out_fd(struct command* cmd, char* file_name, int append) {
    if (cmd->out_fd != STDOUT_FILENO) {
        print_error(DUPLICATED_OUTPUT_REDIRECTION, NULL);
        return 0;
    }
    if (append) cmd->out_fd = open(file_name, O_WRONLY | O_APPEND | O_CREAT, 0600);
    else cmd->out_fd = open(file_name, O_WRONLY | O_TRUNC | O_CREAT, 0600);
    if (cmd->out_fd == -1) {
        print_error(PERMISSION_DENIED, file_name);
        return 0;
    }
    return 1;
}

void redirect(struct command* cmd) {
    dup2(cmd->in_fd, STDIN_FILENO);
    dup2(cmd->out_fd, STDOUT_FILENO);
}

void close_fd(struct command* cmd) {
    if (cmd->in_fd != STDIN_FILENO) close(cmd->in_fd);
    if (cmd->out_fd != STDOUT_FILENO) close(cmd->out_fd);
}

void reset_fd(struct command* cmd) {
    close_fd(cmd);
    cmd->in_fd = STDIN_FILENO;
    cmd->out_fd = STDOUT_FILENO;
}

void reset_fds(struct command* commands, int cnt) {
    for (int i = 0; i < cnt; i++) {
        reset_fd(&commands[i]);
    }
}
