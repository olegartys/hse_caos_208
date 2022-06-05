#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <time.h>
#include <sys/types.h>
#include <wait.h>
#include <unistd.h>

#include <pthread.h>

enum {
	ARR_LEN = 10000000U,
	ARR_SIZE = ARR_LEN * sizeof(int),
};

enum {
	THREAD_COUNT = 4,
};

enum {
	L1_CACHE_LINE = 64, // arch-specific
};

struct thread_info {
	pthread_t pt;

	int *arr;
	size_t begin;
	size_t end;

	size_t sum;
} __attribute__((aligned(L1_CACHE_LINE)));
// выравнивание делается для обхода эффекта false sharing, негативно сказывающегося на производительности
// [ {}, {}, {}, {} ]
//   40  , 40, 40, 40
//	 [   ]
// false sharing
// cache
// когерентность

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

static void *func(void *arg) {
	struct thread_info *ti = arg;

	for (size_t i = ti->begin; i < ti->end; i++) {
		ti->sum += ti->arr[i];
	}

	return NULL;
}

int main(void) {
	int ret = 0;

	int *arr = malloc(sizeof(*arr) * ARR_LEN);
	if (!arr) {
		perror("malloc");
		return 1;
	}

	srand(time(NULL));
	fill_arr(arr, ARR_LEN);

	struct thread_info ti[THREAD_COUNT];

	size_t block_size = ARR_LEN / THREAD_COUNT;
	for (size_t i = 0; i < THREAD_COUNT; ++i) {
		size_t begin_block = block_size * i;
		size_t end_block = block_size * (i + 1);

		struct thread_info *cur_ti = &ti[i];
		cur_ti->begin = begin_block;
		cur_ti->end = end_block;
		cur_ti->arr = arr;

		ret = pthread_create(&cur_ti->pt, NULL, func, cur_ti);
		if (ret < 0) {
			perror("pthread_create");
			goto exit_malloc;
		}
	}

	size_t sum = 0;
	for (size_t i = 0; i < THREAD_COUNT; i++) {
		pthread_join(ti[i].pt, NULL);
		sum += ti[i].sum;
	}

	printf("%zu\n", sum);

exit_malloc:
	free(arr);

	return ret;
}
