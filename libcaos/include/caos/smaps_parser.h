#pragma once

#include <stdio.h>
#include <stdint.h>

size_t parse_smaps_pss(const char *s);
size_t parse_smaps_rss(const char *s);
char *parse_smaps_library(const char *s);
char **parse_smaps(FILE *f, size_t *pss, size_t *rss, size_t *libs_size);
