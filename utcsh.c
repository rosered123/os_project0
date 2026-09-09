/*
  utcsh - The UTCS Shell

  <Put your name and CS login ID here>
*/

/* Read the additional functions from util.h. They may be beneficial to you
in the future */
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
void eval (struct Command *cmd){
  if (cmd->args == NULL || cmd->args[0] == NULL)
  {
    return; 
  }

  if (try_exec_builtin (cmd))
  {
    return; 
  }

  exec_external_cmd (cmd);
}

int try_exec_builtin (struct Command *cmd){
  if (cmd->args[0] == NULL) {
    return 1;
  }
  if (strcmp (cmd->args[0], "exit") == 0)
  {
    if (cmd->args[1] != NULL)
    {
      fprintf(stderr, "An error has occured\n");
      exit(1);
    }
    exit(0);
  }
  if (strcmp (cmd->args[0], "cd") == 0)
  {
    if (cmd->args[1] == NULL || cmd->args[2] != NULL)
    {
      fprintf(stderr, "An error has occured\n");
      exit(1);
    }
    if (chdir(cmd->args[1]) != 0)
    {
      fprintf(stderr, "An error has occured\n");
      exit(1);
    }
    return 1;
  }
  if (strcmp (cmd->args[0], "path") == 0) {

  }
  return 0;
}
void exec_external_cmd (struct Command *cmd);

/* Main REPL: read, evaluate, and print. This function should remain relatively
   short: if it grows beyond 60 lines, you're doing too much in main() and
   should try to move some of that work into other functions. */
int main (int argc, char **argv){
  set_shell_path (default_shell_path);

  /* These two lines are just here to suppress certain warnings. You should
   * delete them when you implement Part 1.4 */
  (void) argc;
  (void) argv;
  char *buffer = NULL;
  size_t bufsize = 0;
  ssize_t input;

  while (1){
      printf ("%s", prompt);
      //printf ("If you see these lines, you are probably running the shell "
        //      "skeleton. Exiting to prevent terminal spam.\n");
      //exit (1883);

      /* Read */
      input = getline(&buffer, &bufsize, stdin);
      /* Evaluate */
      if (input == -1) {
        exit(1);
      }
      buffer[strcspn(buffer, "\n")] = '\0';

      if (strcmp(buffer, "exit") == 0) {
        exit(0);
      } else {
        printf(buffer);
      }
      /* Print (optional) */
    }
  
  free(buffer);
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
  (void) cmdline;
  int n = 8;
  char **arr = malloc(n * sizeof(char*));
  if (arr == NULL) {
    exit(1);
  }
  char *token = strtok(cmdline, " ");
  int i = 0;

  while(token != NULL){
    if (i == n) {
      n = n * 2;
      char *temp = realloc(arr, n);
      if (temp == NULL) {
        fprintf(stderr, "An error has occured. Not enough memory for tokens");
        free(arr);
        exit(1);
      }
      arr = temp;
    }
    arr[i] = &token;
    i++;
    token = strtok(NULL, " ");
  }
  arr[i] = NULL;
  return arr;
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
  struct Command dummy = {.args = tokens, .outputFile = NULL};
  return dummy;
}

/** Evaluate a single command
 *
 * Both built-ins and external commands can be passed to this function--it
 * should work out what the correct type is and take the appropriate action.
 */
void eval (struct Command *cmd)
{
  (void) cmd;
  return;
}

/** Execute built-in commands
 *
 * If the command is a built-in command, execute it and return 1 if appropriate
 * If the command is not a built-in command, do nothing and return 0
 */
int try_exec_builtin (struct Command *cmd)
{
  (void) cmd;
  return 0;
}

/** Execute an external command
 *
 * Execute an external command by fork-and-exec. Should also take care of
 * output redirection, if any is requested
 */
void exec_external_cmd (struct Command *cmd)
{
  (void) cmd;
  return;
}
