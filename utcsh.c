/*
  utcsh - The UTCS Shell

  Kiera Pattani kmp4782
  Shreya Goel sg63353
*/

/* Read the additional functions from util.h. They may be beneficial to you
in the future */
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Global variables */
/* The array for holding shell paths. Can be edited by the functions in util.c*/
char shell_paths[MAX_ENTRIES_IN_SHELLPATH][MAX_CHARS_PER_CMDLINE];
static char prompt[] = "utcsh> "; /* Command line prompt */
static char *default_shell_path[2] = {"/bin", NULL};
/* End Global Variables */

/* Convenience struct for describing a command. Modify this struct as you see
 * fit--add extra members to help you write your code. */
struct Command
{
  char **args;      /* Argument array for the command */
  char *outputFile; /* Redirect target for file (NULL means no redirect) */
};

/* Here are the functions we recommend you implement */

char **tokenize_command_line (char *cmdline);
struct Command parse_command (char **tokens);
void eval (struct Command *cmd);
int try_exec_builtin (struct Command *cmd);
void exec_external_cmd (struct Command *cmd);

/* Main REPL: read, evaluate, and print. This function should remain relatively
   short: if it grows beyond 60 lines, you're doing too much in main() and
   should try to move some of that work into other functions. */
int main (int argc, char **argv)
{
  set_shell_path (default_shell_path);

  char *buffer = NULL;
  size_t bufsize = 0;
  ssize_t input;
  FILE *file;
  if (argc == 2) {
    file = fopen(argv[1], "r");
    if (file == NULL) {
      fprintf(stderr, "An error has occurred. File is invalid\n");
      exit(1);
    }
  } else if(argc == 1) {
    file = stdin;
    } else {
    fprintf(stderr, "An error has occurred. Too many arguments when calling this script\n");
    exit(1);
  }
  while (1) {
    if(file == stdin) {
      printf ("%s", prompt);
    }
      /* Read */
      input = getline(&buffer, &bufsize, file);
      if (input == -1) {
        // ctrl + d or EOF
        exit(0);
      }
      buffer[strcspn(buffer, "\n")] = '\0';
      char **tokens = tokenize_command_line(buffer);
      /* Evaluate */
      struct Command cmd = parse_command(tokens);
      eval(&cmd);
      /* Print (optional) */
      free(tokens);
  }
  return 0;
}

/* NOTE: In the skeleton code, all function bodies below this line are dummy
implementations made to avoid warnings. You should delete them and replace them
with your own implementation. */

/** Turn a command line into tokens with strtok
 *
 * This function turns a command line into an array of arguments, making it
 * much easier to process. First, you should figure out how many arguments you
 * have, then allocate a char** of sufficient size and fill it using strtok()
 */
char **tokenize_command_line (char *cmdline)
{
 int n = 8;
 char **tokens = malloc(n * sizeof(char*)); 
  if (tokens == NULL) {
    fprintf(stderr, "An error has occured\n");
  }

  char *token = strtok(cmdline, " ");
  if (token == NULL) {
    fprintf(stderr, "An error has occurred. No command\n");
    return NULL;
  }
  int i = 0;
  if (strcmp(token, "exit") == 0) {
    if(strtok(NULL, " ") != NULL) {
      fprintf(stderr, "An error has occurred. Exit has more commands\n");
      return NULL;
    }
    tokens[0] = token;
    return tokens;
  }
  if (strcmp(token, "cd") == 0){
    tokens[0] = token;
    token = strtok(NULL, " ");
    if (token == NULL) {
      fprintf(stderr, "An error has occurred. Not enough arguments for cd\n");
      return NULL;
    }
    tokens[1] = token;
    if(strtok(NULL, " ") != NULL) {
      fprintf(stderr, "An error has occurred. cd can only have one argument\n");
      return NULL;
    }
    return tokens;
  }
  
  while (token != NULL) {
    if (i == n - 1) {
      n = n * 2;
      char **temp = realloc(tokens, n * sizeof(char*));
      if (temp == NULL) {
        fprintf(stderr, "An error has occurred. Not enough memory for tokens\n");
        return NULL;
      }
      tokens = temp;
    }
    tokens[i] = token;
    i++;
    token = strtok(NULL, " ");
  }
  tokens[i] = NULL;
  return tokens;
}

/** Turn tokens into a command.
 *
 * The `struct Command` represents a command to execute. This is the preferred
 * format for storing information about a command, though you are free to change
 * it. This function takes a sequence of tokens and turns them into a struct
 * Command.
 */
struct Command parse_command (char **tokens)
{
  if (tokens == NULL) {
    fprintf(stderr, "An error has occurred. Nothing to parse\n");
    return;
  }
  struct Command cmd = {.args = tokens, .outputFile = NULL};
  return cmd;
}

/** Evaluate a single command
 *
 * Both built-ins and external commands can be passed to this function--it
 * should work out what the correct type is and take the appropriate action.
 */
void eval (struct Command *cmd)
{
  if (try_exec_builtin(cmd) == 0) {
    exec_external_cmd(cmd);
  }
}

/** Execute built-in commands
 *
 * If the command is a built-in command, execute it and return 1 if appropriate
 * If the command is not a built-in command, do nothing and return 0
 */
int try_exec_builtin (struct Command *cmd)
{
  if (cmd->args[0] == NULL) {
    fprintf(stderr, "An error has occurred. No commands\n");
    return 1;
  }
  if (strcmp(cmd->args[0], "exit") == 0) {
    exit(0);
    return 1;
  }
  if (strcmp(cmd->args[0], "cd") == 0) {
    if(chdir(cmd->args[1]) == -1){
      fprintf(stderr, "An error has occurred. Error changing directory.");
    }
    return 1;
  }
  if(strcmp(cmd->args[0], "path") == 0){
    set_shell_path(cmd->args[1]);
    return 1;
  }
  return 0;
}

/** Execute an external command
 *
 * Execute an external command by fork-and-exec. Should also take care of
 * output redirection, if any is requested
 */
void exec_external_cmd (struct Command *cmd)
{
  int pid = fork();
  if (pid < 0) {
    fprintf(stderr, "An error has occurred with forking\n");
  } else if (pid == 0) {
    // is a child
    execv(cmd->args[0], cmd->args); //finish shell paths
    fprintf(stderr, "An error has occurred. execv failed\n");
    return;
  } else {
    // parent
    int status;
    waitpid(pid, &status, 0);
  }
}
