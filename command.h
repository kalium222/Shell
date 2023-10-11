#ifndef COMMAND_H_
#define COMMAND_H_

#define MAX_ARGC 20
// #define MAX_WORD_SIZE 10

struct command
{
    char *argv[MAX_ARGC];
    int argc;
    int bg;
    int in_fd;
    int out_fd;
    int append;
};

/**
 * @brief initialize a command
 * 
 * @param cmd 
 */
void init_command(struct command* cmd);
// /**
//  * @brief Get the new command object, initialize it.
//  * 
//  * @return struct command* 
//  */
// struct command* new_command(void);
/**
 * @brief init a list of commands
 * 
 * @param cnt 
 * @param commands
 */
void init_commands(struct command* commands, int cnt);
/**
 * @brief malloc and return a list of commands
 * 
 * @param max_cnt 
 * 
 * @return list of commands
 */
struct command* new_commands(int max_cnt);
// /**
//  * @brief free a command
//  * 
//  * @param cmd 
//  */
// void free_command(struct command* cmd);
/**
 * @brief free lists of commands
 * 
 * @param commands 
 * @param cnt 
 */
void free_commands(struct command* commands);
/**
 * @brief Set the in fd
 * 
 * @param cmd 
 * @param file_name 
 * 
 * @return int 0 if failed, 1 if success
 */
int set_in_fd(struct command* cmd, char* file_name);
/**
 * @brief Set the out fd object
 * 
 * @param cmd 
 * @param file_name 
 * @param append 
 * 
 * @return int 0 if failed, 1 if success
 */
int set_out_fd(struct command* cmd, char* file_name, int append);
/**
 * @brief set STD to fd of a command
 * 
 * @param cmd 
 */
void redirect(struct command* cmd);
/**
 * @brief close all the fds of a command
 * 
 * @param cmd 
 */
void close_fd(struct command* cmd);
/**
 * @brief reset all the fds of a commands
 * 
 * @param cmd 
 */
void reset_fd(struct command* cmd);
/**
 * @brief reset all the fds of a list of commands
 * 
 * @param cmd 
 * @param cnt 
 */
void reset_fds(struct command* cmd, int cnt);

#endif
