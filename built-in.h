#ifndef BUILT_IN_H_
#define BUILT_IN_H_

#include <unistd.h>

struct built_in
{
    char *name;
    int in_child_proc;
    int (*func)(int argc, char **argv);
};

/**
 * @brief exit the shell
 * 
 * @param argc 
 * @param argv 
 * @return int return 1 to exit the shell
 */
int built_in_exit(int argc, char **argv);
/**
 * @brief change directory
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int built_in_cd(int argc, char **argv);
/**
 * @brief print working directory
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int built_in_pwd(int argc, char **argv);
/**
 * @brief whether the command is built in
 * 
 * @param cmd 
 * @return int 
 */
int is_built_in(char *cmd);
/**
 * @brief whether the command is shuold be run in child process
 * 
 * @param cmd 
 * @return int 
 */
int is_run_in_child(char *cmd);
/**
 * @brief execute built in command
 * 
 * @param cmd 
 * @param argc 
 * @param argv 
 * @return int 
 */
int execute_built_in(char *cmd, int argc, char **argv);

extern const struct built_in built_in_list[];

#endif
