#include <stdio.h>
#include <sys/time.h>
#include <time.h>

// В программе определна функция bar.
// Требуется замерить время ее исполнения.

static void bar(void) {
	int i;
	unsigned sum;

	for (i = 0; i < 100000; i++) {
		sum += i;
	}

	printf("%u\n", sum);
}

int main(void) {
	printf("first time calling printf\n");

	clock_t t1 = clock();
	bar();
	clock_t t2 = clock();

	struct timeval tv1;
	gettimeofday(&tv1, NULL);
	bar();
	struct timeval tv2;
	gettimeofday(&tv2, NULL);

	printf("%f\n", (double)(t2 - t1) / CLOCKS_PER_SEC);
	printf("%f\n", (double)(tv2.tv_usec - tv1.tv_usec) / 1000000 + (tv2.tv_sec - tv1.tv_sec));

	return 0;
}
