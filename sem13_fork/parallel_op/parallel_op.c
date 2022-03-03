#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <time.h>
#include <sys/types.h>
#include <wait.h>
#include <unistd.h>

enum {
	ARR_LEN = 10000000,
	ARR_SIZE = ARR_LEN * sizeof(int),
};

enum {
	PROCESS_COUNT = 100,
};

static void fill_arr(int *arr, size_t size) {
	for (size_t i = 0; i < size; i++) {
		arr[i] = rand();
	}
}

static void print_arr(int *arr, size_t size) {
	for (size_t i = 0; i < size; i++) {
		printf("%d\n", arr[i]);
	}
}

static void process(int *arr, size_t begin, size_t end) {
	for (size_t i = begin; i != end; ++i) {
		arr[i] *= 2;
	}
}

int main(void) {
	int *arr = mmap(NULL, ARR_SIZE, PROT_READ | PROT_WRITE,
			MAP_ANONYMOUS | MAP_SHARED, -1, 0);
	if (arr == MAP_FAILED) {
		perror("mmap");
		return 1;
	}

	srand(time(NULL));
	fill_arr(arr, ARR_LEN);

	size_t block_size = ARR_LEN / PROCESS_COUNT;
	for (size_t i = 0; i < PROCESS_COUNT; ++i) {
		size_t begin_block = block_size * i;
		size_t end_block = block_size * (i + 1);

		pid_t p = fork();
		switch (p) {
			case -1:
				perror("fork");
				munmap(arr, ARR_SIZE);
				return 1;

			case 0:
				process(arr, begin_block, end_block);
				munmap(arr, ARR_SIZE);
				return 0;

			default:
				printf("Process (%d) works with range [%zu:%zu]\n", p, begin_block, end_block);
				break;
		}
	}

	while (wait(NULL) > 0);
	munmap(arr, ARR_SIZE);

	return 0;
}
