#include <caos/smaps_parser.h>

#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

static int isnumber(const char *number)  {
	while (*number) {
		if (!isdigit(*number++)) {
			return 0;
		}
	}

	return 1;
}

int main(void) {
	DIR *d = opendir("/proc");
	if (!d) {
		perror("opendir");
		return 1;
	}

	// обходим директорию proc
	// для каждой entry, которая состоит только из цифр
	// открываем в ней файлик smaps
	// и передаем его в функцию из libcaos char **parse_smaps(FILE *f, size_t *pss, size_t *rss, size_t *libs_size);
	struct dirent *dentry = NULL;
	size_t all_pss = 0;
	size_t all_rss = 0;
	do {
		errno = 0;
		dentry = readdir(d);
		if (dentry == NULL) {
			if (errno != 0) {
				perror("readdir");
				continue;
			}

			break;
		}
		// puts(dentry->d_name); // /proc/{name}/smaps

		if (!isnumber(dentry->d_name)) {
			continue;
		}

		size_t current_pss = 0;
		size_t current_rss = 0;
		size_t libs_size = 0;
		char buf[PATH_MAX] = { 0 };

		// snprintf(buf, PATH_MAX, "/proc/%s/smaps", dentry->d_name);
		snprintf(buf, PATH_MAX, "/%s/%s/%s", "proc", dentry->d_name, "smaps");
		FILE *f = fopen(buf, "r");
		if (!f) {
			perror("fopen");
			continue;
		}

		parse_smaps(f, &current_pss, &current_rss, &libs_size);
		all_pss += current_pss;
		all_rss += current_rss;

		fclose(f);
	} while (dentry != NULL);

	closedir(d);

	printf("pss=%zu kb, rss=%zu kb\n", all_pss, all_rss);

	return 0;
}
