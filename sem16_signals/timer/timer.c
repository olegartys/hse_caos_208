#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

static volatile sig_atomic_t cnt;
static volatile sig_atomic_t to_print;

static void bar(int signo, siginfo_t *si, void *ctx) {
	// реентребальными (reentrant)
	if (signo == SIGALRM) {
		alarm(1);
		cnt++;
		// CISC vs RISC
		// load-store: i++:
		// 1. load i from memory into reg
		// 2. reg++
		// 3. store i into memory
	} else if (signo == SIGUSR2) {
		cnt = 0;
	} else if (signo == SIGUSR1) {
		to_print = 1;
	}

	// FIXME: нельзя вызывать printf в обработчиках сигналов
	// printf("I am in bar, signo = %d\n", signo);
	// sleep(5);
}

int main(void) {
	// Программа раз в секунду увеличивает счетчик секунд, прошедших с момента запуска.
	// По SIGUSR2 значение счетчика сбрасывается, по SIGUSR1 выводится на экран
	printf("my pid is %d\n", getpid());

	struct sigaction sa;
	sa.sa_flags = SA_SIGINFO /*| SA_NODEFER*/;
	sa.sa_sigaction = bar;
	sa.sa_restorer = NULL;
	sigemptyset(&sa.sa_mask);

	// default handler
	// user-specified handler
	// SIG_IGN
	// blocked

	if (sigaction(SIGUSR1, &sa, NULL) < 0) {
		perror("Sigaction fault");
		return 1;
	}

	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGALRM);
	if (sigaction(SIGUSR2, &sa, NULL) < 0) {
		perror("Sigation 2 error");
		return 1;
	}

	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGUSR2);
	if (sigaction(SIGALRM, &sa, NULL) < 0) {
		perror("Sig alarm error");
		return 1;
	}

	alarm(1);

	while (1) {
		pause();
		if (to_print == 1) {
			printf("%d\n", cnt);
			to_print = 0;
		}
	}

	// busy loop
	// SIGALRM
	return 0;
}
