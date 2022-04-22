#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

#define TEMP_FAILURE_RETRY(expr) ({ \
	typeof(expr) ret; \
	do { \
		ret = expr; \
	} while (ret == -1 && errno == EINTR); \
	ret; }) \

static void handler(int signo) {
	printf("hello\n");
	alarm(5);
}

int main(void) {
	// программа в бесконечном цикле читает stdin и выводит прочитанное на stdout
	// можно пользоваться только системными вызовами (например, read и write)

	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));

	sa.sa_handler = handler;

	if (sigaction(SIGALRM, &sa, NULL) < 0) {
		perror("sigaction");
		return 1;
	}

	alarm(5);

	char buf[64] = { 0 };
	while (1) {
		ssize_t nr = TEMP_FAILURE_RETRY(read(STDIN_FILENO, buf, sizeof(buf)));
		if (nr == 0) {
			return 0;
		} else if (nr == -1) {
			perror("read");
			return 1;
		}

		ssize_t nw = TEMP_FAILURE_RETRY(write(STDOUT_FILENO, buf, nr));
		if (nw == -1) {
			perror("write");
			return 1;
		}
	}

	return 0;
}
