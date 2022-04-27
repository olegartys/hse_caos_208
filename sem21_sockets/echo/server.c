// berkley socket API -- программный интерфейс для общения между двумя endpoint-ами
// IPC (inter-process communication): pipe(fd[2]), mmap,    socket,     signal, file (flock)

#include <stdio.h>
#include <errno.h>

#include <sys/socket.h>
#include <sys/un.h> 
#include <unistd.h>

enum {
	MAX_ACCEPTED  = 10,
	BUF_SIZE = 1024,
};

int main(void) {
	int ret = 0;

	// socket -> bind -> listen -> accept

	// OSI (IP -- 3, TCP -- 4): TCP/IP
	int sfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sfd < 0) {
		perror("socket");
		return 1;
	}

	struct sockaddr_un addr;
	addr.sun_family = AF_UNIX;
	snprintf(addr.sun_path, sizeof(addr.sun_path), "socket_path");

	if (unlink(addr.sun_path) == -1) {
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

	struct sockaddr_un caddr;
	socklen_t caddrlen = sizeof(caddr);

	int cfd = accept(sfd, (struct sockaddr *)&caddr, &caddrlen);
	if (cfd == -1) {
		ret = errno;
		perror("accept");
		goto exit_cfd;
	}

	// poll, select + O_NONBLOCK

	char buf[BUF_SIZE] = { 0 };
	ssize_t nr = -1;
	do {
		nr = read(cfd, buf, sizeof(buf));
		if (nr < 0) {
			// TODO: need to add retries or exit on error
			perror("read");
			continue;
		}

		ssize_t nw = write(cfd, buf, nr);
		if (nw != nr) {
			perror("write");
			continue;
		}
	} while (nr != 0);

exit_cfd:
	close(cfd);

exit_sfd:
	close(sfd);

	return ret;
}
