// В аргументах командной строки передается список файлов.
// Требуется последовательно вывести их содержимое на экран с использованием системных вызовов read и write.

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

// Функция читает файл по пути path, возвращает указатель
// на буфер с содержимым файла, в file_size записывает размер файла
//
// Буфер должен быть освобожден на вызывающей стороне
//
// При ошибке возвращает NULL
static char *read_file(const char *path, off_t *file_size) {
	char *data = NULL;
	off_t size = -1;
	ssize_t nb = -1;
	int fd = -1;

	// Системный вызов open используется для открытия файла
	// Возвращает файловый дескриптор -- целое неотрицательное число,
	// которое для данного процесса идентифицирует файл.
	fd = open(path, O_RDONLY);
	if (fd == -1) {
		return NULL;
	}

	// С каждым открытым в процессе файлом в linux связана специальная структура
	// в ядре -- struct file
	// Она содержит поле, обозначающее текущее смещение внутри файла
	//
	// lseek используется для изменения смещения в файле, возвращает новое смещение
	// Один из способов подсчета размера файла -- изменение смещения на конец файла
	size = lseek(fd, 0, SEEK_END);
	if (size == -1) {
		goto fail;
	}

	// Меняем позицию обратно на начало файла
	if (lseek(fd, 0, SEEK_SET) == -1) {
		goto fail;
	}

	// Выделяем буфер по размеру файла
	data = calloc(size, 1);
	if (data == NULL) {
		goto fail;
	}

	// Читаем весь файл в буфер
	nb = read(fd, data, size);
	if (nb != size) {
		goto fail;
	}

	*file_size = size;

	// Дескриптор нужно закрыть
	close(fd);

	return data;

fail:
	if (data != NULL) {
		free(data);
		data = NULL;
	}

	close(fd);

	return data;
}

int main(int argc, char *argv[]) {
	char *file_data = NULL;
	off_t file_size = -1;
	ssize_t nb = -1;
	int i;

	// argc и argv используются для работы с аргументами командной строки
	// argv -- массив C-style строк, argc -- его размер
	// Если argc != 0, argv[0] содержит команду, которой была запущена программа
	// Стандарт к этому не обязывает, но в *nix обычно это так
	// В курсе будем считать, что это так

	for (i = 1; i < argc; i++) {
		// read_file выделяет буфер на куче, его нужно освободить
		// на вызывающей стороне
		file_data = read_file(argv[i], &file_size);
		if (!file_data) {
			perror("error reading file");
			continue;
		}

		// Выводим содержимое файла на экран -- для этого
		// передаем номер дескриптора stdout
		nb = write(STDOUT_FILENO, file_data, file_size);
		if (nb != file_size) {
			perror("error writing file");
		}

		free(file_data);
	}

	return 0;
}
