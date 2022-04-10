// origin: https://www.gnu.org/software/libc/manual/html_node/Non_002datomic-Example.html

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

struct two_words {
    int a;
    int b;
};

static volatile struct two_words memory;

static void handler(int signum) {
   printf ("%d,%d\n", memory.a, memory.b);
   alarm(1);
}

int main(void) {
    struct two_words zeros = { 0, 0 }, ones = { 1, 1 };
    memory = zeros;

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));

    sa.sa_handler = handler;
    if (sigaction(SIGALRM, &sa, NULL) < 0) {
        perror("sigaction");
        return 1;
    }

    alarm(1);

    while (1) {
       memory = zeros;
       memory = ones;
    }

    return 0;
}
