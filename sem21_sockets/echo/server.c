// berkley socket API -- программный интерфейс для общения между двумя endpoint-ами
// IPC (inter-process communication): pipe(fd[2]), mmap,    socket,     signal, file (flock)

// lib.so
// ожидание события на дескрипторе: select, poll, epoll (linux), kqeuque (bsd) (+ O_NONBLOCK)
// fd: read, write, close, open, mmap, poll, select
// мультиплексирование

#include "dynarray.h"

#include <stdio.h>
#include <errno.h>

#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h> 
#include <unistd.h>

enum {
	MAX_ACCEPTED  = 10,
	BUF_SIZE = 1024,
};

static int communicate(int fd) {
	char buf[BUF_SIZE] = { 0 };

	ssize_t nr = read(fd, buf, sizeof(buf));
	if (nr < 0) {
		perror("read");
		return nr;
	}

	// FIXME: Здесь может быть busyloop с EAGAIN, потому что вызывающая сторона не проверяет,
	// что дескриптор готов к записи
	ssize_t nw = 0;
	do {
		nw = write(fd, buf + nw, nr);
		nr -= nw;
	} while (nw > 0);

	if (nw < 0) {
		perror("write");
	}

	return nw;
}

static int set_nonblock(int fd) {
	int flags;
	if ((flags = fcntl(fd, F_GETFL, 0)) < 0) {
		flags = 0;
	}

	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int main(void) {
	int ret = 0;

	// socket -> bind -> listen -> accept

	// OSI (IP -- 3, TCP -- 4): TCP/IP
	// address:port (myhost.ru:80 8080)
	int sfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sfd < 0) {
		perror("socket");
		return 1;
	}

	if (set_nonblock(sfd) < 0) {
		fprintf(stderr, "Error setting non-block\n");
		goto exit_sfd;
	}

	struct sockaddr_un addr;
	addr.sun_family = AF_UNIX;
	snprintf(addr.sun_path, sizeof(addr.sun_path), "/tmp/socket_path");

	if (unlink(addr.sun_path) == -1 && errno != ENOENT) {
		ret = errno;
		perror("unlink");
		goto exit_sfd;
	}

	if (bind(sfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
		ret = errno;
		perror("bind");
		goto exit_sfd;
	}

	if (listen(sfd, MAX_ACCEPTED) == -1) {
		ret = errno;
		perror("listen");
		goto exit_sfd;
	}

	struct dynarr *fd_arr = dynarr_init();
	if (!fd_arr) {
		ret = ENOMEM;
		fprintf(stderr, "Error initializing fds array\n");
		goto exit_sfd;
	}

	// Добавляем файловый дескриптор серверного сокета.
	// Если на нем произойдет событие, это означает, что к нам подключается
	// новый клиент, и нужно вызвать accept и добавить его в fd_arr
	dynarr_insert(fd_arr, (struct pollfd){ sfd, POLLIN, 0 });

	while (1) {
		ret = poll(fd_arr->data, fd_arr->size, -1);
		if (ret < 0) {
			perror("poll");
			goto exit_fd_arr;
		} else if (ret == 0) {
			// should never happen
			fprintf(stderr, "Poll timeout\n");
			continue;
		}

		printf("%d descriptors got events\n", ret);

		if (fd_arr->data[0].revents & POLLIN) {
			// хочет подключиться новый клиент
			struct sockaddr_un caddr;
			socklen_t caddrlen = sizeof(caddr);
			int cfd = accept(sfd, (struct sockaddr *)&caddr, &caddrlen);
			if (cfd == -1) {
				perror("accept");
				continue;
			}

			if (set_nonblock(cfd) < 0) {
				fprintf(stderr, "Error setting non-block on client descriptor\n");
				continue;
			}

			// доабвляем его в массив. Теперь мы будем ждать события от него
			ret = dynarr_insert(fd_arr, (struct pollfd){ cfd, POLLIN | POLLHUP /*| POLLOUT */, 0 });
			if (ret != 0) {
				close(cfd);
				fprintf(stderr, "Error inserting new client data into array %d\n", ret);
				continue;
			}

			printf("Got new client, fd = %d\n", cfd);
		} else if (fd_arr->data[0].revents & POLLERR) {
			// произошла какая-то ошибка
			fprintf(stderr, "Error happened on server fd\n");
			goto exit_fd_arr;
		}

		// смотрим, нет ли событий от клиентов
		int i;
		for (i = 1; i < fd_arr->size; i++) {
			int revents = fd_arr->data[i].revents;
			int cfd = fd_arr->data[i].fd;

			if (revents & POLLERR) {
				fprintf(stderr, "Error on client %d socket\n", cfd);
				continue;
			}

			if (revents & POLLHUP) {
				printf("Client %d disconnected\n", cfd);
				dynarr_remove(fd_arr, cfd);
				close(cfd);
				continue;
			}

			// FIXME: communicate делает запись в дескриптор, но здесь мы не проверяем, что
			// дескриптор готов к записи.
			if (revents & POLLIN) {
				ret = communicate(cfd);
				if (ret < 0) {
					fprintf(stderr, "Error communicating with client %d, ret = %d\n", cfd, ret);
					continue;
				} else if (ret == 0) {
					printf("Client %d disconnected\n", cfd);
					dynarr_remove(fd_arr, cfd);
					close(cfd);
					continue;
				}
			}
		}
	}

exit_fd_arr:
	for (int i = 0; i < fd_arr->size; i++) {
		close(fd_arr->data[i].fd);
	}

	dynarr_destroy(fd_arr);

exit_sfd:
	close(sfd);

	return ret;
}
