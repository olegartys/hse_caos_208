#pragma once

#include <pthread.h>

// node (data, next) -> node -> ... -> NULL

struct bq_node {
	int data;
	struct bq_node *next;
};

struct bq {
	pthread_mutex_t lock;
	pthread_cond_t cv;

	struct bq_node *head;
	struct bq_node *tail;
};

extern struct bq *bq_create(void);
extern void bq_destroy(struct bq *);

// добавляет в конец
extern int bq_push(struct bq *, int);

// возвращает элемент из головы и удаляет его
extern int bq_pop(struct bq *);
