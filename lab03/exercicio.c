#define _GNU_SOURCE
#include <stdlib.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sched.h>
#include <stdio.h>
#include <pthread.h>

// The child thread will execute this function
void *threadFunction ( void *var )
{
     int *variavel;
     variavel = (int *)var;
     *variavel = *variavel + 1;
     printf("Variable (var), changed in thread: \nvar = %d\n", *variavel);
     printf("\n--Child thread exiting!\n\n");
}

int main()
{
     int var = 1;
     pthread_t thread;
     
     printf("Variable (var), created in main process: \nvar = %d\n", var);

     // Call the clone system call to create the child thread
     printf( "\n--Creating child thread!\n\n" );
     pthread_create(&thread, NULL, threadFunction, (void *)&var);
     
     // Wait for the child thread to terminate
     pthread_join(thread, NULL);
     printf("Variable (var), after thread change (in main process): \nvar = %d\n", var);
     return 0;
}	     
