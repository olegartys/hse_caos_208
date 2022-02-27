#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>

int main(void) {
	pid_t p = fork();
	switch (p) {
		case -1:
			perror("fork");
			break;

		case 0:
			printf("I am in child, pid=%d, ppid=%d\n", getpid(), getppid());
			return 10;

		default: {
			printf("I am in parent, waiting for child %d to finish\n", p);

			int status;
			if (waitpid(p, &status, 0) == -1) {
				perror("waitpid");
				return 1;
			}

			if (!WIFEXITED(status)) {
				printf("Child finished abnormally\n");
				return 1;
			}

			printf("Child (%d) exited with status %d\n", p, WEXITSTATUS(status));

			return 0;
		}
	}
}
