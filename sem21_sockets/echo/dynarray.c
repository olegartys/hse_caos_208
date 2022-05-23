#include "dynarray.h"

#include <errno.h>
#include <stdlib.h>

enum {
	CAPACITY_FACTOR = 2,
};

struct dynarr *dynarr_init(void) {
	struct dynarr *p = malloc(sizeof(*p));
	if (p != NULL) {
		p->size = 0;
		p->capacity = 1;
		p->data = NULL;
	}

	return p;
}

void dynarr_destroy(struct dynarr *d) {
	if (d != NULL) {
		free(d->data);
	}

	free(d);
}

int dynarr_insert(struct dynarr *d, struct pollfd el) {
	if (d->size >= d->capacity - 1) {
		d->capacity *= CAPACITY_FACTOR;
		void *p = realloc(d->data, d->capacity);
		if (!p) {
			return -errno;
		}

		d->data = p;
	}
	d->data[d->size] = el;
	d->size++;
	return 0;
}

void dynarr_remove(struct dynarr *d, int fd) {
	for (int i = 0; i < d->size; ++i) {
		if (d->data[i].fd == fd) {
			d->size--;
			d->data[i] = d->data[d->size];
			return;
		}
	}
}
