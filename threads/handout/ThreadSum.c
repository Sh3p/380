/*
 * Filename	: ThreadSum.c
 * Author	: Christian Shepperson
 * Course	: CSCI 380-01
 * Assignment	: Threads
 * Description	: Write a Pthreads program that computes a parallel
 * sum of an array A of N random integers in the range [0, 4]
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/******* Globals *******/
int* g_array; 
int g_num; 
int g_threadcount; 

/******* Function prototypes *******/
void*
threads (void* id);

long
parallelSum();

long
serialSum();

/******* Main *******/

int
main (int argc, char* argv[])
{
	printf("p ==> ");

	scanf("%d", &g_threadcount);

	printf("N ==> ");

	scanf("%d", &g_num);

	g_array = malloc(g_num * sizeof(int));

	for(int i = 0; i < g_num; ++i)
	{
		g_array[i] = rand() % 5;	
	}

	printf("Parallel sum:  %ld\n", parallelSum());
	printf("Serial sum:    %ld\n", serialSum());

	free(g_array);
}

/*
 *Thread id
 * return - Sum of integers in the thread's block of elements
 */
void*
thread(void* id)
{
	long sum = 0;

	long block = g_num / g_threadcount;

	long start = block * (long)id;

	long end = start + block;

	for(long i = start; i < end; ++i)
	{
		sum += g_array[i];;
	}

	return (void*)sum;
}

/*
 * Compute sum of ints in array g_array using parallel threads
 * return - sum of values in array
 */
long
parallelSum()
{
	pthread_t* threads = malloc(g_threadcount * sizeof(pthread_t));

	for(long id = 0; id < g_threadcount; ++id)
	{
		pthread_create(&threads[id], NULL, thread, (void*)id);
	}
	long sum = 0;
	for(long id = 0; id < g_threadcount; ++id)
	{
		void* returnVal;
		pthread_join(threads[id], &returnVal);
		sum += (long)returnVal;
	}
	free(threads);
	return sum;
}

/*
 * Compute sum of ints in array g_array using main thread
 * return - sum of values in array
 */
long
serialSum()
{
	long sum = 0;
	for(int i = 0; i < g_num; ++i)
	{
		sum += g_array[i];
	}
	return sum;
}
