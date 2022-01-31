#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>

int main(void) {
	DIR *d = opendir("/proc");
	if (!d) {
		perror("opendir");
		return 1;
	}

	struct dirent *dentry = NULL;
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

		puts(dentry->d_name);
	} while (dentry != NULL);

	closedir(d);

	return 0;
}
