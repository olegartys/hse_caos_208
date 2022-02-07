#include "../include/caos/common.h"

#include <errno.h>
#include <stdlib.h>

// Обертка над getline, которая либо всегда завершается успешно,
// либо завершает программу.
size_t xgetline(char **line, size_t *len, FILE* f) {
	errno = 0;
	ssize_t ret = getline(line, len, f);
	if (ret == -1) {
		if (errno != 0) {
			perror("getline failed");
			exit(EXIT_FAILURE);
		}
	}
	return ret;
}

// Обертка над realloc, которая либо всегда завершается успешно,
// либо завершает программу.
void *xrealloc(void *p, size_t size) {
	// При p == NULL, realloc ведет себя так же, как malloc
	void *ret = realloc(p, size);
	if (ret == NULL) {
		perror("realloc failed");
		exit(EXIT_FAILURE);
	}
	return ret;
}
