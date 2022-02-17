#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

enum {
	MAX_SZ = 1 << 10, // 1KB
};

static void help(const char *name) {
	fprintf(stderr, "Usage: %s DIRECTORY\n", name);
}

int main(int argc, char **argv) {
	if (argc != 2) {
		help(argv[0]);
		return 1;
	}

	const char *dir_path = argv[1];

	DIR *d = opendir(dir_path);
	if (!d) {
		perror("opendir");
		return 1;
	}

	// Получаем файловый дескриптор директории
	int dir_fd = dirfd(d);

	struct dirent *dentry = NULL;
	do {
		// получаем dentry = directory entry
		errno = 0;
		dentry = readdir(d);
		if (!dentry) {
			if (errno) {
				perror("readdir");
				continue;
			}

			break;
		}

		// dentry -- описывает компонент в пути (например, /usr/bin/vim)
		// он нигде не персистится, формируется в рантайме и хранится только в оперативной памяти
		// в каждой dentry есть имя файла
		//
		// '.' означает текущую директорию, '..' -- директорию на уровень выше
		const char *name = dentry->d_name;
		if (name[0] == '.') {
			continue;
		}

		// читаем inode. У каждого файла есть свой inode. В нем хранится метаинформация о файле.
		// имя файла не хранится, оно находится в dirent.h
		//
		// он персистится на диске. inode есть так же у директории, директория -- такой же файл
		//
		// fstatat принимает в качестве последнего аргумента флаги, среди которых есть AT_SYMLINK_NOFOLLOW
		// он определяет поведение вызова функции, если файл "name" является символьной ссылкой: запрашивать
		// информацию о файле, на который указывает ссылка, или о самой ссылке. См. разницу между stat() и lstat()
		struct stat st;
		if (fstatat(dir_fd, name, &st, AT_SYMLINK_NOFOLLOW) != 0) {
			perror("stat");
			continue;
		}

		// st_size имеет тип off_t он знаковый!
		if (st.st_size >= MAX_SZ) {
			printf("Removing file \"%s/%s\"\n", dir_path, name);

			// Удаляем файл. На самом деле просто уменьшаем количество
			// жестких ссылок на inode
			if (unlinkat(dir_fd, name, 0 /* flags */) != 0) {
				perror("unlink");
			}
		}
	} while (dentry != NULL);

	closedir(d);

	return 0;
}
