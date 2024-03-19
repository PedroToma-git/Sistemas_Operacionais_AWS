#define _GNU_SOURCE
#include <stdlib.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sched.h>
#include <stdio.h>
#include <pthread.h>

// 64kB stack
#define FIBER_STACK 1024*64

// The child thread will execute this function
void *threadFunction ( void *var )
{
     int *variavel;
     variavel = (int *)var;
     printf("%d", *variavel);
     printf("child thread exiting\n");
}

int main()
{
     int var = 1;
     pthread_t thread;

     printf( "Creating child thread\n" );

     // Call the clone system call to create the child thread
     pthread_create(&thread, NULL, threadFunction, (void *)&var);

     // Wait for the child thread to exit

     pthread_join(thread, NULL);
     return 0;
}	     
