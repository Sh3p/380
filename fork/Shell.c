/*
  Filename   : Shell.c
  Author     : Christian Shepperson
  Course     : CSCI 380-01
  Assignment : Prelude to Shell
  Description: Creates 2 processes and prints different PID associated with it
*/
#include <stdio.h>
#include <stdlib.h>

// For strerror
#include <string.h>

// For fork, getpid, execve, read, write, close,
//   and other Linux system calls
#include <unistd.h>
// For errno
#include <errno.h>



int main(){
  char strArray[80];

  printf("shell>");
  fgets( strArray, 80, stdin);
  strtok(strArray, "\n");

  pid_t pid = fork();

 if (pid < 0)
  {
    fprintf (stderr, "fork error (%s) -- exiting\n",
	     strerror (errno));
    exit (1);
  }
  /*
  gives each process a corresponding printout, either can be printed first or second
  */
  if (pid == 0)
  {
   printf ("Child says:  PID = %d, parent PID = %d, '%s'\n", getpid(), getppid(), strArray); 
    exit (0); 
  }
  printf ("Parent says: PID = %d, child PID = %d, parent PID = %d\n", getpid(), pid, getppid() );
  
}