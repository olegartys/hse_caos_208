#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>

enum {
    READ_BUF_SIZE = 100,
    OUT_FILE_MAX_FIZE = READ_BUF_SIZE * 10,
};

#define OUT_FILE_NAME "log.txt"
#define OUT_FILE_NAME_ROTATED OUT_FILE_NAME ".old"

static void read_child_stdout(int fd) {
    FILE *log_file = fopen(OUT_FILE_NAME, "w");
    if (!log_file) {
        perror("fopen");
        return;
    }

    char buf[READ_BUF_SIZE];
    size_t written = 0;
    ssize_t status;
    while ((status = read(fd, buf, READ_BUF_SIZE)) > 0) {
        if (fwrite(buf, 1, status, log_file) != status) {
            perror("fwrite");
            fclose(log_file);
            return;
        }

        written += status;
        if (written >= OUT_FILE_MAX_FIZE) {
            fclose(log_file);

            if (rename(OUT_FILE_NAME, OUT_FILE_NAME_ROTATED) < 0) {
                perror("rename");
                return;
            }

            FILE *log_file = fopen(OUT_FILE_NAME, "w");
            if (!log_file) {
                perror("fopen");
                return;
            }

            written = 0;
        }
    }
}

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

    int pipefd[2];
    // p[0], p[1] : все, что пишется в p[1], может быть прочитано из p[0]
    if (pipe(pipefd) < 0) {
        free(prog_args);
        perror("pipe");
        return 1;
    }

    // 2. создаем процесс и в нем запускаем программу с аргументами
    pid_t p = fork();
    switch (p) {
        case -1:
            perror("fork");
            return 1;

        case 0: {
            close(pipefd[0]);

            if (dup2(pipefd[1], STDOUT_FILENO) != STDOUT_FILENO) {
                perror("dup2");
                return 1;
            }

            if (execvp(prog_name, prog_args) < 0) {
                perror("execvp");
            }

            return 1;
        }

        default: {
            close(pipefd[1]);
            read_child_stdout(pipefd[0]);
        }
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
