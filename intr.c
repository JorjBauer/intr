#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#if HAVE_SYS_WAIT_H
#include <sys/wait.h> // new
#endif
#include <signal.h>


/* For the signal handler. Unfortunate that it's a global. */
pid_t global_child_pid = -1; 

void child_handler(int signal_num)
{
  int status;
  pid_t pid;

  signal( signal_num, child_handler ); /* ... yes, we want more of these! */

  /* fprintf(stderr, "%s\n", sys_siglist[signal_num]); */

  /* SIGCHLD is caught so we can terminate early (the child has finished). */
  if ( signal_num == SIGCHLD ) {
    pid = waitpid( WAIT_MYPGRP, &status, 0 );
    exit( status );
  }

  /* Any other signal should be forwarded to the child. */
  if ( global_child_pid != -1 ) {
    kill( global_child_pid, signal_num );
  }
}


void do_parent(int delay, pid_t pgroup, pid_t child_pid)
{
  int cur_sig;
  int i;

  /* What signals we'll send to non-responding commands, and in what order */
  int signals[7] = { SIGTERM, SIGINT, SIGHUP, SIGQUIT, SIGKILL,
		     SIGSEGV, SIGBUS /* last-ditch efforts... */
  };

  global_child_pid = child_pid;
  /* Grab all signal handlers: we'll pass any signals to the child. */
  for (i=1; i<NSIG; i++) {
    signal( i, child_handler );
    
  }

  sleep(delay);

  for ( cur_sig=0; cur_sig<7; cur_sig++ ) {
#if HAVE_DECL_SYS_SIGNAME
    fprintf( stderr, 
	     "intr: sending child 'sig%s'\n", 
	     sys_signame[signals[cur_sig]] );
#else
    fprintf( stderr, 
	     "intr: sending child signal %d\n", 
	     signals[cur_sig] );
#endif
    /* Make sure the process is running, so we can kill it, and then do so. */
    kill( child_pid, SIGCONT );
    kill( child_pid, signals[cur_sig] );
    sleep( 1 );
  }

  fprintf(stderr, 
	  "intr: unable to kill process. Attempting to kill the process "
	  "group.\n");
  killpg( pgroup, SIGKILL );
}

void do_child(char *argv[])
{
  execvp(argv[0], argv);
  perror( "intr: execvp failed" );
}

int main(int argc, char *argv[])
{
  pid_t child_pid;
  pid_t pgroup = setsid();
  int delay = 5; /* default delay, in seconds */
  char **args;

  /* Error checking */
  if ( argc < 3 ) {
    fprintf( stderr, "intr %s <jorj@jorj.org>\n", PACKAGE_VERSION );
    fprintf( stderr, "Usage: %s <seconds> <command> [arguments]\n", argv[0] );
    exit( -2 );
  }

  delay = atoi(argv[1]);
  if (delay <= 0) {
    fprintf( stderr, "intr: delay must be an integer > 0\n" );
    exit( -3 );
  }

  /* Construct the argument list for the child process. */
  int i;

  /* Construct a new array of arguments that we can NULL-terminate and pass
   * them to execlp. */
  args = malloc( sizeof (char *) * (argc-1) );
  for ( i=2; i<argc; i++ ) {
    args[i-2] = argv[i];
  }
  args[argc-2] = NULL;
  
  if ( (child_pid = fork()) == -1 ) {
    /* Error forking */
    perror( "Failed to fork" );
    exit( -1 );
  } else if ( child_pid == 0 ) {
    /* Child process */
    do_child(args);
  } else {
    /* Parent process */
    do_parent(delay, pgroup, child_pid);
  }

  return 0;
}
