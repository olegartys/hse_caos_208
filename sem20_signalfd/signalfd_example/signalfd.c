#include <stdio.h>
#include <sys/signalfd.h>
#include <signal.h>
#include <unistd.h>

static void do_work(int signo) {
	printf("Got signal %d, do some work\n", signo);
}

int main(void) {
	printf("pid = %d\n", getpid());

	int ret = 0;

	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGUSR1);
	sigaddset(&set, SIGUSR2);

	if (sigprocmask(SIG_BLOCK, &set, NULL) < 0) {
		perror("sigprocmask");
		return 1;
	}

	int sfd = signalfd(-1, &set, 0);
	if (sfd < 0) {
		perror("signalfd");
		return 1;
	}

	// read, write, close, mmap, poll (select, epoll), (ioctl)

	while (1) {
		struct signalfd_siginfo sinfo;
		ssize_t nr = read(sfd, &sinfo, sizeof(sinfo));
		if (nr != sizeof(sinfo)) {
			fprintf(stderr, "read failed, ret = %zd\n", nr);
			ret = 1;
			goto exit_sfd;
		}

		do_work(sinfo.ssi_signo);
	}

exit_sfd:
	close(sfd);

	return ret;
}
