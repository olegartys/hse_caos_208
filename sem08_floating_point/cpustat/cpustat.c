// 1. откуда вообще брать данные о нагрузке?
// 2. считать их в целых числах или в вещественных

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define STAT_PATH "/proc/stat"
#define SAMPLE_TIME_S 1

// пример содержимого файла /proc/cpustat
//
// cpu  218242 4733 275040 73085090 43367 0 16756 0 0 0
// cpu0 22454 1147 102600 18287140 13762 0 964 0 0 0
// cpu1 150637 1508 61322 18225750 12148 0 1500 0 0 0
// cpu2 25762 1104 61062 18272842 7752 0 1554 0 0 0
// cpu3 19387 972 50054 18299358 9703 0 12737 0 0 0
// intr 8629418 37 34453 0 0 0 0 3 0 1 0 0 326458 144 0 698881 46963 0 0 0 0 0 0 0 0 0 75913 69027 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
// ctxt 16366394
// btime 1642211960
// processes 53944
// procs_running 1
// procs_blocked 0
// softirq 3481259 0 449188 16512 340750 720400 0 30447 947268 0 976694

// CPU генерирует с определенной частотой прерывание
// jiffies -- счетчик таких прерываний

// idle_jiffies -- то, сколько времени процессор ничего не делает
// all_jiffies -- то, сколько времени процессор что-то делал

// busy percent: 100 - (idle_jiffies - old_idle_jiffies) / (all_jiffies - old_all_jiffies) * 100

int main(void) {
    FILE *f = fopen(STAT_PATH, "r"); // /proc/stat
    if (!f) {
        perror("error openning file");
        return 1;
    }

    // отключаем буферизацию, чтобы fscanf гарантированно
    // перечитывал файл на каждой итерации
    setvbuf(f, NULL, _IONBF, 0);

    size_t old_idle_jiffies = 0;
    size_t old_all_jiffies = 0;

    // Парсим jiffies из /proc/stat
    while (1) {
        size_t idle_jiffies = 0;
        size_t all_jiffies = 0;

        // Вычитываем токен "cpu"
        char *buf;
        fscanf(f, "%ms", &buf);

        size_t jiffies = 0;
        int i = 0;
        while (fscanf(f, "%zu", &jiffies) == 1) {
            if (i == 3) {
                idle_jiffies += jiffies;
            }

            i++;

            all_jiffies += jiffies;
        }

        free(buf);

        size_t usage = 100 - (idle_jiffies - old_idle_jiffies) * 100 / (all_jiffies - old_all_jiffies);
        float fUsage = 100 - (idle_jiffies - old_idle_jiffies) * 100.f / (all_jiffies - old_all_jiffies);
        double dUsage = 100 - (idle_jiffies - old_idle_jiffies) * 100. / (all_jiffies - old_all_jiffies);

        printf("usage=%zu fUsage=%f dUsage=%lf\n", usage, fUsage, dUsage);

        old_all_jiffies = all_jiffies;
        old_idle_jiffies = idle_jiffies;

        if (fseek(f, 0, SEEK_SET) != 0) {
            perror("Error fseek!");
        }

        sleep(SAMPLE_TIME_S);
    }

    fclose(f);

    return 0;
}
