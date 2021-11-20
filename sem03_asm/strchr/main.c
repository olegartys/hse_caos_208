#include <stdio.h>

extern void my_strchr(const char *, int);

int main(void) {
	char s[256] = { 0 };
	if (!fgets(s, sizeof(s), stdin))  {
		perror("fgets");
		return -1;
	}

	int c = getchar();
	my_strchr(s, c);

	return 0;
}
