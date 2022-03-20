#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
    if (argc == 1) {
        fprintf(stderr, "Wrong args\n");
        return 1;
    }

    // 1. получить аргументы для запуска новой программы
    const char *prog_name = argv[1];
    char **prog_args = NULL;

    prog_args = malloc(sizeof(*prog_args) * argc);
    if (!prog_args) {
        perror("malloc");
        return 1;
    }

    for (size_t i = 0; i < argc -1; ++i) {
        prog_args[i] = argv[i + 1];
    }

    prog_args[argc - 1] = NULL;

    // 2. создаем процесс и в нем запускаем программу с аргументами
    pid_t p = fork();
    switch (p) {
        case -1:
            perror("fork");
            return 1;

        case 0:
            if (execvp(prog_name, prog_args) < 0) {
                perror("execvp");
            }

            return 1;

        default:
            break;
    }

    // 3. дожидаемся ее завершения и получаем код возврата
    int status;
    if (waitpid(p, &status, 0 /* flags */) < 0) {
        perror("waitpid");
        return 1;
    }

    if (!WIFEXITED(status)) {
        fprintf(stderr, "Child finished abnormally\n");
        return 1;
    }

    printf("Child (%d) exited with status %d\n", p, WEXITSTATUS(status));

    return 0;
}
