// Author: Gary M. Zoppetti
// Compile with
//   gcc -Wall HelloPthreads.c -o HelloPthreads -lpthread

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <pthread.h> 

//**

const long MAX_THREADS = 16;

// Accessible to all threads
long threadCount;  

void
printUsage (const char* progName);

void*
sayHello (void* threadId);

//**

int
main (int argc, char* argv[]) 
{
  if (argc != 2)
  {
    printUsage (argv[0]);
    exit (EXIT_FAILURE);
  }

  threadCount = atol (argv[1]);
  if (threadCount <= 0 || threadCount > MAX_THREADS)
  {
    printUsage (argv[0]);
    exit (EXIT_FAILURE);
  }

  pthread_t* threads = malloc (threadCount * sizeof (pthread_t)); 
  
  for (long threadId = 0; threadId < threadCount; ++threadId)  
  {
    // Second arg is attributes object, which we will not use. 
    pthread_create (&threads[threadId], NULL,
		    sayHello, (void *) threadId);
  }

  printf ("Hello from main thread\n");
  
  for (long threadId = 0; threadId < threadCount; ++threadId) 
  {
    void* returnVal;
    pthread_join (threads[threadId], &returnVal);
    printf ("Status = %ld\n", (long) returnVal);
  }

  free (threads);

  // pthread_exit ensures all threads have terminated before
  //   the main thread terminates
  pthread_exit (EXIT_SUCCESS);
}

//**

void*
sayHello (void* threadId) 
{
  long myId = (long) threadId;
  
  sleep (1);
  printf ("Hello from thread %ld of %ld\n", myId, threadCount);
  
  void* retVal = (void *) 311;
  return retVal;
} 

//**

void
printUsage (const char* progName) 
{
  fprintf (stderr, "Usage: %s <number of threads>\n", progName);
  fprintf (stderr, "0 < number of threads <= %ld\n", MAX_THREADS);
}
