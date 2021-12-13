// Программе на вход через аргументы командной строки подается pid процесса.
//
// Требуется:
// 1. Посчитать количество занимаемой процессом памяти по метрикам Rss и Pss
// 2. Вывести отсортированной список используемых библиотек

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

// Обертка над getline, которая либо всегда завершается успешно,
// либо завершает программу.
static size_t xgetline(char **line, size_t *len, FILE* f) {
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
static void *xrealloc(void *p, size_t size) {
	// При p == NULL, realloc ведет себя так же, как malloc
	void *ret = realloc(p, size);
	if (ret == NULL) {
		perror("realloc failed");
		exit(EXIT_FAILURE);
	}
	return ret;
}

static pid_t parse_args(int argc, char *argv[]) {
	pid_t pid = -1;
	switch (argc) {
		case 1:
			pid = getpid();
			break;

		case 2: {
			// На семинаре предлагалось использовать
			// errno для проверки результата парсинга
			// Это неверно, потому что ошибка EINVAL
			// согласно man strtol может не выставляться
			// в зависимости от реализации
			char *end;
			pid = strtol(argv[1], &end, 10);
			if (end == argv[1] || *end != '\0') {
				pid = -1;
			}
			break;
		}
	}

	return pid;
}

static void help(const char *p) {
	fprintf(stderr, "usage: %s [pid]\n", p);
}

// Пример строки: "Pss:                   4 kB"
// Мы хотим выпарсить из нее 4
static size_t parse_smaps_pss(const char *s) {
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
static size_t parse_smaps_rss(const char *s) {
	if (!(s[0] == 'R' && s[1] == 's' && s[2] == 's')) {
		return 0;
	}

	size_t rss = 0;
	sscanf(s, "%*s %zu", &rss);
	return rss;
}

static char *parse_smaps_library(const char *s) {
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

static char **parse_smaps(FILE *f, size_t *pss, size_t *rss, size_t *libs_size) {
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

// Предикат для сортировки строк с помощью qsort
static int comparator(const void *p1, const void *p2) {
	const char *s1 = *(char**)p1;
	const char *s2 = *(char**)p2;
	return strcmp(s1, s2);
}

int main(int argc, char *argv[]) {
	// Парсим аргументы
	pid_t pid = parse_args(argc, argv);
	if (pid < 0) {
		help(argv[0]);
		return EXIT_FAILURE;
	}

	// Формируем путь
	char filename[PATH_MAX] = { 0 };
	snprintf(filename, PATH_MAX, "/proc/%d/smaps", pid);

	// Открываем файл
	FILE *f = fopen(filename, "r");
	if (!f) {
		perror("fopen failed");
		return EXIT_FAILURE;
	}

	// Парсим smaps
	size_t pss = 0, rss = 0, libs_size = 0;
	char **libs = parse_smaps(f, &pss, &rss, &libs_size);
	if (libs == NULL) {
		fprintf(stderr, "Error parsing smaps\n");
		return EXIT_FAILURE;
	}

	// Выводим результат
	printf("pss=%zu kb rss=%zu kb\n", pss, rss);
	if (libs_size > 0) {
		// Сортируем строки
		qsort(libs, libs_size, sizeof(*libs), comparator);
		for (int i = 0; i < libs_size; i++) {
			// Если в имени библиотеки есть имя нашей нашей программы,
			// не выводим его
			//
			// В 100% случаев это работать не будет, хотя бы потому что
			// имя нашей программы может быть в пути до библиотеки
			//
			// Это просто пример демонстрации работы strstr
			if (strstr(libs[i], argv[0]) == NULL) {
				printf("lib[%d] = %s\n", i, libs[i]);
			}

			free(libs[i]);
		}

		free(libs);
	}

	fclose(f);

	return EXIT_SUCCESS;
}
