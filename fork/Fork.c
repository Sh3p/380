// Demonstrate use of fork
// Gary M. Zoppetti

#include <stdio.h>
#include <stdlib.h>

// For strerror
#include <string.h>

// For fork, getpid, execve, read, write, close,
//   and other Linux system calls
#include <unistd.h>
// For errno
#include <errno.h>

int
main ()
{
  printf ("Initial hi from parent\n"); 
  pid_t pid = fork ();
  if (pid < 0)
  {
    fprintf (stderr, "fork error (%s) -- exiting\n",
	     strerror (errno));
    exit (1);
  }
  if (pid == 0)
  {
    printf ("Hi from child\n");
    exit (0); 
  }
  printf ("Hi from parent. Forked child PID is %d.\n", pid); 
  
  return 0;
}
