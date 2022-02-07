#pragma once

#include <stdio.h>
#include <stdint.h>

size_t xgetline(char **line, size_t *len, FILE *f);
void *xrealloc(void *p, size_t size);
