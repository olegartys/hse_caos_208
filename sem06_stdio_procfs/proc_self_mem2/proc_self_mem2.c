#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

static void bar(int x) {
	x += 6;
	printf("%s/%d: x = %d\n", __func__, __LINE__, x);
}

typedef void(bar_func_t)(int);

static void patch_func(int fd) {
	// Перезаписываем код инструкции, чтобы к x в bar
	// прибавилось не 6, а 7
	unsigned char buf[] = { 0x83, 0x45, 0x08, 0x07 };
	int ret = write(fd, buf, sizeof(buf));
	if (ret != sizeof(buf)) {
		perror("Errror");
		return;
	}
}

int main(void) {
	int ret = 0;

	int fd = open("/proc/self/mem", O_RDWR);
	if (fd < 0) {
		perror("open failed");
		return EXIT_FAILURE;
	}

	bar_func_t *bar_addr = bar;
	printf("bar address=0x%"PRIxPTR"\n", (uintptr_t)bar_addr);

	// 0x15 -- смещение инструкции add от начала функции bar
	// можно увидеть в objdump
	if (lseek(fd, (off_t)bar_addr + 0x15, SEEK_SET) < 0) {
		ret = errno;
		perror("lseek fail");
		goto exit;
	}

	puts("Before patch");
	bar(5);

	puts("Enter to patch");
	getchar();

	patch_func(fd);

	puts("After patch");
	bar(5);

exit:
	close(fd);
	return ret;
}
