#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

int kill(pid_t pid, int sig);

int main(int argc, char **argv)
{
  if (argc != 2){
    exit(1);
  }
  pid_t pid = (pid_t) argv[1];

  if (kill(pid, SIGUSR1) < 0){
    exit(1);
  }
  return 0;
}
