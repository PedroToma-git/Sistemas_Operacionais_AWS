#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#define BUFFER_SIZE 4


int  value = 5;

int main()
{
	pid_t pid;
        int p[2];

	if (pipe(p) < 0) { /* error in pipe creation */
	        exit(1);
        }

	pid = fork();

	if (pid == 0) { /* child process */
		printf("Entrei no filho!\n");
		value += 15;
		write(p[1], &value, BUFFER_SIZE);
		close(p[1]);
		printf ("CHILD: value = %d\n",value); /* LINE A */
		return 0;
	}
	else if (pid > 0) { /* parent process */
		wait(NULL);
		read(p[0], &value, BUFFER_SIZE);
		printf ("PARENT: value = %d\n",value); /* LINE A */
		return 0;
	}
}

