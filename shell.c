#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#include "command.h"
#include "built-in.h"
#include "shell_errors.h"

// ----------- Constants -----------
#define MAX_INPUT_SIZE 1024
#define MAX_FILE_NAME_SIZE 30
#define MAX_COMMAND_COUNT 20
#define PROMPT "sh $ "

// ----------- Global -----------
int has_child = 0;

// ----------- Declarations -----------

/**
 * @brief do the shell loop
 * 
 * @param exit_status the address of the exit status
 */
void shell_loop(int* exit_status);
/**
 * @brief Get the input
 * 
 * @param input 
 */
void get_input(char* input);
/**
 * @brief Creates a tokenized form of the input with spaces to separate words
 * 
 * @param input 
 * @param tokenized 3 times the size of input
 */
void tokenize(char *input, char *tokenized);
/**
 * @brief Parse the command lind, handle redirection, pipe, background
 * 
 * @param tokenizable The tokenizable string of the command line to parse
 * @param commands The array of commands to store the parsed commands
 * @param cmd_cnt commands count
 * 
 * @return int 1 if success, 0 if error, and need to continue the shell loop
 */
int parse(char *tokenizable, struct command* commands, int* cmd_cnt);
/**
 * @brief execute all the commands in the pipe
 * 
 * @param commands 
 * @param cmd_cnt 
 * @return int exit_status
 */
int execute_pipe(struct command* commands, int cmd_cnt);
/**
 * @brief When CTRL-C, exit child process and continue the shell loop
 * 
 * @param sig 
 */
void sigint_handler(int sig);

// ----------- Main -----------

int main(void) {
    int exit_status = 0;
    signal(SIGINT, sigint_handler);
    shell_loop(&exit_status);
    return exit_status;
}

// ----------- Implementations -----------

void shell_loop(int* exit_status) {
    char input[MAX_INPUT_SIZE];
    char tokenizable[MAX_INPUT_SIZE*3];
    struct command* commands = new_commands(MAX_COMMAND_COUNT);
    int command_count = 0;
    do {
        printf(PROMPT);
        fflush(stdout);
        get_input(input);
        tokenize(input, tokenizable);
        if (!parse(tokenizable, commands, &command_count)) continue;
        *exit_status = execute_pipe(commands, command_count);
    } while (!*exit_status);
    free_commands(commands);
}

void get_input(char* input) {
    memset(input, 0, MAX_INPUT_SIZE);
    if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) {
        printf("exit\n");
        exit(0);
    }
}

void tokenize(char *input, char *tokenized) {
    memset(tokenized, 0, MAX_INPUT_SIZE*3);
    int j = 0; // iterator of tokenized
    for (int i=0; i<(int)strlen(input); i++) {
        if (input[i]=='>' || input[i]=='<' || input[i]=='|') {
            tokenized[j++] = ' ';
            tokenized[j++] = input[i];
            tokenized[j++] = ' ';
        } else {
            tokenized[j++] = input[i];
        }
    }
    tokenized[j] = '\0';
}

int parse(char *tokenizable, struct command* commands, int* cmd_cnt) {
    reset_fds(commands, *cmd_cnt);
    int i = 0; // iterator of word
    int j = 0; // iterator of command
    char *token = strtok(tokenizable, " \n"); // Split the input by space or newline
    while (token != NULL) {
        if (strcmp(token, ">") == 0) {
            token = strtok(NULL, " \n");
            if (strcmp(token, ">") == 0) {
                token = strtok(NULL, " \n");
                commands[j].append = 1;
                if (!set_out_fd(&commands[j], token, commands[j].append)) return 0;
            } else {
                commands[j].append = 0;
                if (!set_out_fd(&commands[j], token, commands[j].append)) return 0;
            }
        } else if (strcmp(token, "<") == 0) {
            token = strtok(NULL, " \n");
            if (!set_in_fd(&commands[j], token)) return 0;
        } else if (strcmp(token, "|") == 0) {
            // next command in pipe
            commands[j].argv[i] = NULL;
            commands[j].argc = i;
            i = 0;
            j++;
        } else {
            commands[j].argv[i] = token;
            i++;
        }
        token = strtok(NULL, " \n");
    }
    commands[j].argv[i] = NULL;
    commands[j].argc = i;
    *cmd_cnt = j+1;
    return 1;
}

int execute_pipe(struct command* commands, int cmd_cnt) {
    int exit_status = 0;
    int (*pipefd)[2] = malloc(sizeof(int[2]) * (size_t)(cmd_cnt-1));
    if (pipefd == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    pid_t *child_pid = malloc(sizeof(pid_t) * (size_t)cmd_cnt);
    if (child_pid == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    int *child_status = malloc(sizeof(int) * (size_t)cmd_cnt);
    if (child_status == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    // create pipes and set in_fd and out_fd
    for (int i=0; i<cmd_cnt-1; i++) {
        if (pipe(pipefd[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
        commands[i].out_fd = (pipefd[i][1]);
        commands[i+1].in_fd = (pipefd[i][0]);
    }
    // create child processes
    for (int i=0; i<cmd_cnt; i++) {
        if (commands[i].argc == 0) continue; // empty command
        if (is_built_in(commands[i].argv[0]) && !is_run_in_child(commands[i].argv[0])) {
            execute_built_in(commands[i].argv[0], commands[i].argc, commands[i].argv);
            continue;
        }
        switch (child_pid[i] = fork()) {
        case -1: // Error
            perror("fork failed");
            break;
        case 0: // Child process
            redirect(&commands[i]); // Redirect in_fd and out_fd
            // Close all pipes to ensure no waiting
            for (int j=0; j<cmd_cnt; j++) {
                close_fd(&commands[j]);
            }
            // built-in command
            if (is_built_in(commands[i].argv[0])) {
                exit_status = execute_built_in(commands[i].argv[0], commands[i].argc, commands[i].argv);
                exit(exit_status);
            }
            execvp(commands[i].argv[0], (char * const *)commands[i].argv);
            // execvp only returns if error
            print_error(COMMAND_NOT_FOUND, commands[i].argv[0]);
            exit(0); // Exit the child process only
            break;
        default: // Parent process
            has_child = 1;
            break;
        }
    }
    for (int i=0; i<cmd_cnt; i++) {
        close_fd(&commands[i]);
    }
    for (int i=0; i<cmd_cnt; i++) {
        // TODO: what to return?
        // TODO: bg
        waitpid(child_pid[i], &child_status[i], 0);
        // if (child_status[i] != 0) exit_status = 1;
    }
    free(pipefd);
    free(child_pid);
    free(child_status);
    has_child = 0;
    return exit_status;
}

void sigint_handler(int sig) {
    // TODO: execvp() will not get the signal(...) result. why?
    if (sig!=SIGINT) {
        perror("sigint_handler: wrong signal");
        exit(EXIT_FAILURE);
    }
    printf("\n");
    if (!has_child) {
        printf(PROMPT);
        fflush(stdout);
    }
}
