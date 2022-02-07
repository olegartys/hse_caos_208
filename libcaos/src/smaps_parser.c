#include "../include/caos/smaps_parser.h"

#include "../include/caos/common.h"

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// Пример строки: "Pss:                   4 kB"
// Мы хотим выпарсить из нее 4
size_t parse_smaps_pss(const char *s) {
	if (!(s[0] == 'P' && s[1] == 's' && s[2] == 's')) {
		return 0;
	}

	size_t pss = 0;
	// * означает, что мы не хотим сохранять значение,
	// подходящее этому формату
	//
	// Т.е. мы читаем строку "Pss:" вникуда, а значение
	// после пробельных символов -- в pss.
	sscanf(s, "%*s %zu", &pss);
	return pss;
}

// Аналогично Rss выше
size_t parse_smaps_rss(const char *s) {
	if (!(s[0] == 'R' && s[1] == 's' && s[2] == 's')) {
		return 0;
	}

	size_t rss = 0;
	sscanf(s, "%*s %zu", &rss);
	return rss;
}

char *parse_smaps_library(const char *s) {
	// Функция проверяет, является ли символ 16ричной цифрой
	if (!isxdigit(s[0])) {
		return NULL;
	}

	// %ms говорит семейству функций scanf динамически выделить
	// память на куче. Это удобно, когда мы не знаем
	// длину выпаршиваемой строки
	char *bits = NULL;
	char *path = NULL;
	if (sscanf(s, "%*s %ms %*s %*s %*s %ms", &bits, &path) != 2) {
		goto fail;
	}

	if (strlen(bits) < 4 || strlen(path) < 1) {
		goto fail;
	}

	if (bits[2] != 'x') {
		goto fail;
	}

	if (path[0] == '[') {
		goto fail;
	}

	free(bits);

	return path;

fail:
	free(bits);
	free(path);

	return NULL;
}

char **parse_smaps(FILE *f, size_t *pss, size_t *rss, size_t *libs_size) {
	char **libs = NULL;
	char *line = NULL;
	size_t len = 0;
	ssize_t nr = -1;

	do {
		// Читаем строку
		errno = 0;
		nr = xgetline(&line, &len, f);
		if (nr == -1) {
			break;
		}

		// Парсим из нее pss, rss и имя библиотеки
		// Имя библиотеки выделено на куче
		*pss += parse_smaps_pss(line);
		*rss += parse_smaps_rss(line);
		char *lib_path = parse_smaps_library(line);
		if (!lib_path) {
			continue;
		}

		// Добавляем имя библиотеки в динамический массив
		// Для этого с помощью realloc выделяем память под
		// еще один элемент char *
		libs = xrealloc(libs, ++*libs_size * sizeof(*libs));

		// Записываем в новый выделенный элемент указатель
		// на строку с именем библиотеки
		libs[*libs_size - 1] = lib_path;
	} while (nr != -1);

	// getline выделяет память в куче, мы должны ее освобождать
	free(line);

	return libs;
}
