#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include "util.h"


void handle_signal(int sig){

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
  printf("%s", pid);

  struct sigaction sa;
  sa.sa_handler = handle_signal;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;

  sigaction(SIGINT, &sa, NULL);

  while(1){
    printf("%s", "Still here\n");
    nanosleep()
  }

  ssize_t bytes;
  const int STDOUT = 1;
  bytes = write(STDOUT, "Nice try.\n", 10);
  if(bytes != 10)
    exit(-999);
  return 0;
}


