#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

/* A dummy test program that will ignore all signals. Used for testing intr. */

int main(int argc, char *argv[])
{
  int i;
  for (i=0; i<NSIG; i++){
    signal(i, SIG_IGN);
  }
  while (1) {
    sleep(1);
    printf("lalala. Still running.\n");
  }
}

