#include "bq.h"

#include <stdio.h>
#include <stdlib.h>

// struct bq_node {
// 	int data;
// 	struct bq_node *next;
// };

// struct bq {
// 	struct bq_node *head;
// 	struct bq_node *tail;
// };

struct bq *bq_create(void) {
	struct bq *q = malloc(sizeof(*q));
	if (!q) {
		perror("malloc");
		return NULL;
	}

	q->head = q->tail = NULL;

	pthread_mutex_init(&q->lock, NULL);
	pthread_cond_init(&q->cv, NULL);

	return q;
}

void bq_destroy(struct bq *q) {
	pthread_cond_destroy(&q->cv);
	pthread_mutex_destroy(&q->lock);

	free(q);
}

int bq_push(struct bq *q, int data) {
	struct bq_node *n = malloc(sizeof(*n));
	if (!n) {
		perror("malloc");
		return -1;
	}

	n->data = data;
	n->next = NULL;

	pthread_mutex_lock(&q->lock);

	if (!q->head) {
		q->head = n;
	} else {
		q->tail->next = n;
	}

	q->tail = n;

	pthread_cond_signal(&q->cv);
	pthread_mutex_unlock(&q->lock);

	return 0;
}

int bq_pop(struct bq *q) {
	pthread_mutex_lock(&q->lock);
	while (!q->head) { // spurious wakeup
		pthread_cond_wait(&q->cv, &q->lock);
	}

	struct bq_node *head = q->head;
	int data = head->data;

	q->head = head->next;

	pthread_mutex_unlock(&q->lock);

	free(head);

	return data;
}
