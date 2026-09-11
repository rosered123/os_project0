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
#include <fcntl.h>

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
  bool valid; /*Way to keep track of if command is valid or not.*/
};

/* Here are the functions we recommend you implement */

char **tokenize_command_line (char *cmdline);
struct Command parse_command (char **tokens);
void eval (struct Command *cmd);
int try_exec_builtin (struct Command *cmd);
pid_t exec_external_cmd (struct Command *cmd);
char ***split_and(char **cmdline, int *n);
void run_command_line(struct Command *cmds, int n);

/* Main REPL: read, evaluate, and print. This function should remain relatively
   short: if it grows beyond 60 lines, you're doing too much in main() and
   should try to move some of that work into other functions. */
int main (int argc, char **argv){
  set_shell_path (default_shell_path);
  char *buffer = NULL;
  size_t bufsize = 0;
  ssize_t input;
  FILE *file;

  //find out if input is a file or stdin
  if (argc == 2) {
    file = fopen(argv[1], "r");
    if (file == NULL) {
      fprintf(stderr, "An error has occurred\n");
      exit(1);
    }
  } else if(argc == 1) {
    file = stdin;
  } else {
    fprintf(stderr, "An error has occurred\n");
    exit(1);
  }


  bool prev_input = false;
  while (1) {
    if(file == stdin) {
      printf ("%s", prompt);
    }

    /* Read */
    input = getline(&buffer, &bufsize, file);
    if (input == -1) {
      if(ferror(file)) { // there was a read error
        fprintf(stderr, "An error has occurred\n");
        free(buffer);
        exit(1);
      }

      if (argc == 2 && !prev_input) { // empty file (invalid)
        fprintf(stderr, "An error has occurred\n");
        free(buffer);
        exit(1);
      }

      //reached end of file
      free(buffer);
      exit(0);
    }

    if (file != stdin) {
      if (fseek(file, 0, SEEK_CUR) != 0) {
        fprintf(stderr, "An error has occurred\n");
        free(buffer);
        exit(1);
      }
     }

    prev_input = true;
    buffer[strcspn(buffer, "\n")] = '\0'; //strip newline character
    //breaking string into tokens
    char **tokens = tokenize_command_line(buffer);
    if (tokens == NULL) {
      continue;
    }

    /* Evaluate */
    int num_commands;
    //split tokens based on &
    char ***commands = split_and(tokens, &num_commands); 
    if (commands == NULL) {
      free(tokens);
      continue;
    }

    struct Command cmds[num_commands];

    //parse each command
    for (int i = 0; i < num_commands; i++) {
      cmds[i] = parse_command(commands[i]);
      if (!cmds[i].valid) {
        fprintf(stderr, "An error has occurred\n");
        exit(0);
      }
    }

    //execute commands
    run_command_line(cmds, num_commands);

    for (int i = 0; i < num_commands; i++) {
        free(commands[i]);
    }

    free(commands);
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
char **tokenize_command_line(char *cmdline){
  int capacity = 8;
  int count = 0;
  char **tokens = malloc(capacity * sizeof(char *));

  if (tokens == NULL) {
      return NULL;
  }

  char *p = cmdline;

  while (*p != '\0') {
    //skip leading whitespace and tabs
    while (*p == ' ' || *p == '\t') {
      p++;
    }

    if (*p == '\0') {
      break;
    }

    char *token_str;
    //handle & as a single token
    if (*p == '&') {
      token_str = malloc(2);

      if (token_str == NULL) {
        fprintf(stderr, "An error has occurred\n");
        free(tokens);
        return NULL;
      }

      token_str[0] = '&';
      token_str[1] = '\0';
      p++;
    } else {
      char *start = p;

      while (*p != '\0' && *p != ' ' && *p != '\t' && *p != '&') {
          p++;
      }

      size_t len = p - start;
      token_str = malloc(len + 1);

      if (token_str == NULL) {
          fprintf(stderr, "An error has occurred\n");
          free(tokens);
          return NULL;
      }

      memcpy(token_str, start, len);
      token_str[len] = '\0';
    }
    //reallocate memory if needed
    if (count == capacity - 1) {
      capacity *= 2;
      char **temp = realloc(tokens, capacity * sizeof(char *));

      if (temp == NULL) {
        fprintf(stderr, "An error has occurred\n");
        free(token_str);
        free(tokens);
        return NULL;
      }

      tokens = temp;
    }
    tokens[count++] = token_str;
  }
  tokens[count] = NULL;
  return tokens;
}

/** Turn tokens into a command.
 *
 * The `struct Command` represents a command to execute. This is the preferred
 * format for storing information about a command, though you are free to change
 * it. This function takes a sequence of tokens and turns them into a struct
 * Command.
 */
struct Command parse_command (char **tokens){
  struct Command cmd = {.args = tokens, .outputFile = NULL, .valid = true};
  for (int i = 0; tokens[i] != NULL; i++) {

    if (strcmp(tokens[i], ">") == 0) {

      if (i == 0) {
        cmd.valid = false;
        return cmd;
      }

      if (tokens[i+1] == NULL) {
        cmd.valid = false;
        return cmd;
      }

      if (tokens[i+2] != NULL) {
        cmd.valid = false;
        return cmd;
      }

      cmd.outputFile = tokens[i+1];
      tokens[i] = NULL;
    }
  }
  return cmd;
}

/** Evaluate a single command
 *
 * Both built-ins and external commands can be passed to this function--it
 * should work out what the correct type is and take the appropriate action.
 */
void eval (struct Command *cmd){
  if (!cmd->valid) {
    return;
  } else if (try_exec_builtin(cmd) == 0) {
    pid_t pid = exec_external_cmd(cmd);
    if (pid > 0) {
      int status;
      waitpid(pid, &status, 0); 
    }
  }
}

/** Execute built-in commands
 *
 * If the command is a built-in command, execute it and return 1 if appropriate
 * If the command is not a built-in command, do nothing and return 0
 */
int try_exec_builtin (struct Command *cmd){
  if (cmd->args[0] == NULL) {
    return 1;
  }

  if (strcmp(cmd->args[0], "exit") == 0) {
    //checking if extra arguments after exit (should be no arguments)
    if (cmd->args[1] != NULL) {
      fprintf(stderr, "An error has occurred\n");
      return 1;
    }
    exit(0);
    return 1;
  }

  if (strcmp(cmd->args[0], "cd") == 0) {
    //must have only one argument
    if (cmd->args[1] == NULL || cmd->args[2] != NULL) {
      fprintf(stderr, "An error has occurred\n");
      return 1;
    }

    if (chdir(cmd->args[1]) == -1) {
      fprintf(stderr, "An error has occurred\n");
      return 1;
    }
    return 1;
  }

  if(strcmp(cmd->args[0], "path") == 0){
    set_shell_path(&cmd->args[1]);
    return 1;
  }

  return 0;
}

/** Execute an external command
 *
 * Execute an external command by fork-and-exec. Should also take care of
 * output redirection, if any is requested
 */
pid_t exec_external_cmd (struct Command *cmd){
  pid_t pid = fork();

  if (pid < 0) {
    fprintf(stderr, "An error has occurred\n");
    return -1;
  } else if (pid == 0) {

    if (cmd->outputFile != NULL) {
      int fd = open(cmd->outputFile, O_WRONLY | O_CREAT | O_TRUNC, 00700);

      if (fd == -1) {
        fprintf(stderr, "An error has occurred\n");
        exit(1);
      }

      if(dup2(fd, STDOUT_FILENO) == -1) {
        fprintf(stderr, "An error has occurred\n");
        exit(1);
      }

      if (dup2(fd, STDERR_FILENO) == -1) {
        fprintf(stderr, "An error has occurred\n");
        exit(1);
      }

      close(fd);
    }

    if (is_absolute_path(cmd->args[0]) != 0) { 
      execv(cmd->args[0], cmd->args);
    } else {
       for (int i = 0; i < MAX_ENTRIES_IN_SHELLPATH; i++) {

        if (shell_paths[i][0] == '\0') {
            continue;  
        }

        char *full_path = exe_exists_in_dir(shell_paths[i], cmd->args[0], false);   

        if (full_path != NULL) {
          execv(full_path, cmd->args);
          free(full_path);
          exit(1);
        }
      }
    }
    fprintf(stderr, "An error has occurred\n");
    exit(1); // terminate child
  }
  return pid;  /* parent: no wait here */
}

char ***split_and(char **cmdline, int *n){
  int capacity = 8;
  int count = 0;
  char ***commands = malloc(capacity * sizeof(char **));

  if (commands == NULL) {
    return NULL;
  }

  int start = 0;
  for (int i = 0; ; i++) {
    if (cmdline[i] == NULL || strcmp(cmdline[i], "&") == 0) {
      int length = i - start;

      char **command = malloc((length + 1) * sizeof(char *));
      if (command == NULL) {
          free(commands);
          return NULL;
      }

      for (int j = 0; j < length; j++) {
          command[j] = cmdline[start + j];
      }

      command[length] = NULL;
      if (count == capacity) {
        capacity *= 2;
        char ***temp = realloc(commands, capacity * sizeof(char **));

        if (temp == NULL) {
          free(command);
          free(commands);
          return NULL;
        }
        commands = temp;
      }
      commands[count++] = command;
      if (cmdline[i] == NULL) {
        break;
      }
      start = i + 1;
    }
  }
  *n = count;
  return commands;
}

//runs commands sequentially if built in, otherwise concurrently
void run_command_line(struct Command *cmds, int n) {
  pid_t pids[n];
  int num_pids = 0;

  for (int i = 0; i < n; i++) {
    if (cmds[i].args[0] == NULL) {
      continue; 
    }

    if (!try_exec_builtin(&cmds[i])) {
      pid_t pid = exec_external_cmd(&cmds[i]);
      if (pid > 0) {
        pids[num_pids++] = pid;
      }
    }
  }

  for (int i = 0; i < num_pids; i++) {
    int status;
    waitpid(pids[i], &status, 0);
  }
}
