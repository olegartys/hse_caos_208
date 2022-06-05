#pragma once

#include <poll.h>
#include <stddef.h>

struct dynarr {
	struct pollfd *data;

	size_t size;
	size_t capacity;
};

extern struct dynarr *dynarr_init(void);
extern void dynarr_destroy(struct dynarr *);

extern int dynarr_insert(struct dynarr *, struct pollfd);
extern void dynarr_remove(struct dynarr *, int fd);
