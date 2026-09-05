#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include "util.h"
#include <string.h>

typedef void handler_t(int);

void handle_signal(int sig){
  ssize_t bytes;
  const int STDOUT = 1;
  bytes = write(STDOUT, "Nice try.\n", 10);
  if(bytes != 10)
    exit(-999);
}

void handle_sigusr1(int sig){
  ssize_t bytes;
  const int STDOUT = 1;
  bytes = write(STDOUT, "exiting\n", 8);
  if (bytes != 8)
    exit(-999);
  exit(1);
}
/*
 * First, print out the process ID of this process.
 *
 * Then, set up the signal handler so that ^C causes
 * the program to print "Nice try.\n" and continue looping.
 *
 * Finally, loop forever, printing "Still here\n" once every
 * three seconds.
 */
int main(int argc, char **argv)
{
  pid_t pid = getpid();
  printf("%d\n", pid);

  handler_t *old_handler = signal_action(SIGINT, handle_signal);
  handler_t *old_sigusr1_handler = signal_action(SIGUSR1, handle_sigusr1);

  struct timespec sleep_amt;
  sleep_amt.tv_sec = 3;
  sleep_amt.tv_nsec = 0;

  while(1){
    printf("Still here\n");
    nanosleep(&sleep_amt, NULL);
  }
  return 0;
}


