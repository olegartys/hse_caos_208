#include "bq.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct thread_info {
	volatile int stopped;
	struct bq *q;
};

// генерирует целые числа
static void *producer(void *arg) {
	struct thread_info *ti = arg;
	struct bq *q = ti->q;

	while (!ti->stopped) {
		int data = rand();
		bq_push(q, data);
		sleep(2);
	}

	return NULL;
}

// их вычитывает и печатает на экран
static void *consumer(void *arg) {
	struct thread_info *ti = arg;
	struct bq *q = ti->q;

	while (!ti->stopped) {
		int data = bq_pop(q);
		printf("%d\n", data);
	}

	return NULL;
}

int main(void) {
	struct bq *q = bq_create();
	if (!q) {
		fprintf(stderr, "error creating bq\n");
		return 1;
	}

	srand(time(NULL));

	pthread_t consumer_tr;
	struct thread_info consumer_ti = { 0, q };
	if (pthread_create(&consumer_tr, NULL, consumer, &consumer_ti) < 0) {
		perror("pthread_create");
		return 1;
	}

	pthread_t producer_tr;
	struct thread_info producer_ti = { 0, q };
	if (pthread_create(&producer_tr, NULL, producer, &producer_ti) < 0) {
		perror("pthread_create");
		pthread_join(consumer_tr, NULL);
		bq_destroy(q);
		return 1;
	}

	getchar();

	consumer_ti.stopped = 1;
	pthread_join(consumer_tr, NULL);

	producer_ti.stopped = 1;
	pthread_join(producer_tr, NULL);

	bq_destroy(q);

	return 0;
}
