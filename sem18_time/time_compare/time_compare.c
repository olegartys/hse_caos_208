#define _XOPEN_SOURCE

#include <stdio.h>
#include <time.h>

#define FMT "%Y-%m-%d %H:%M:%S"

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Wrong arguments!\n");
        return 1;
    }

    const char *required_date = argv[1];
    time_t current_time = time(NULL);

    struct tm tm;
    if (!strptime(required_date, FMT , &tm)) {
        perror("strptime");
        return 1;
    }

    time_t required_time = mktime(&tm);
    if (required_time == -1) {
        perror("mktime");
        return 1;
    }

    if (required_time < current_time) {
        puts("0");
    } else {
        puts("1");
    }

    return 0;
}
