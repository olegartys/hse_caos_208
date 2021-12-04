// В аргументах командной строки передается список файлов.
// Требуется последовательно вывести их содержимое на экран с использованием системных вызовов read и write.

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

// В cat1.c объяснения разных штук, посмотрите туда

// Функция выводит в stdout файл по пути path
// При успехе возвращает 0, иначе -1
static int print_file(const char *path) {
	enum { BUFFER_SIZE = 1024 };
	char buf[BUFFER_SIZE] = { 0 };
	int nr = -1;
	int nw = -1;
	int fd = -1;

	fd = open(path, O_RDONLY);
	if (fd < 0) {
		return fd;
	}

	do {
		nr = read(fd, buf, BUFFER_SIZE);
		if (nr == -1) {
			close(fd);
			return nr;
		}

		nw = write(STDOUT_FILENO, buf, nr);
		if (nr != nw) {
			close(fd);
			return -1;
		}
	} while (nr > 0);

	close(fd);

	return 0;
}

int main(int argc, char **argv) {
	int ret = -1;
	int i;

	for (i = 1; i < argc; i++) {
		ret = print_file(argv[i]);
		if (ret != 0) {
			perror("error printing file");
			return ret;
		}
	}

	return 0;
}
