#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#define BUFFER_SIZE 4  // buffer size for read() and write()

int value = 5;

int main()
{
	pid_t pid;
        int p[2]; // 2 index array for pipeline usage

	if (pipe(p) < 0) { /* error in pipe creation */
	        exit(1);
        }

	pid = fork();

	if (pid == 0) { /* child process */
		close(p[0]); // closing file descriptor for reading (child)
		printf("Entrei no filho!\n");
		value += 15;
		write(p[1], &value, BUFFER_SIZE); // writing in pipe file descriptor
		close(p[1]); // closing fd for writing (child)
		printf ("CHILD: value = %d\n",value); /* LINE A */
		return 0;
	}
	else if (pid > 0) { /* parent process */
		wait(NULL);
		read(p[0], &value, BUFFER_SIZE); // reading fd from pipe
		close(p[1]); // closing fd for writing (parent)
		close(p[0]); // closing fd for reading (parent)
		printf ("PARENT: value = %d\n",value); /* LINE A */
		return 0;
	}
}

