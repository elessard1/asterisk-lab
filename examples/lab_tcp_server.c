#include <asterisk.h>
#include <asterisk/cli.h>
#include <asterisk/module.h>

#include <arpa/inet.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>

struct server {
	pthread_t thread;
	int pipefd[2];
	int sockfd;
};

static struct server global_server;

static int server_init(struct server *s)
{
	struct sockaddr_in addr;
	int pipefd[2] = {-1, -1};
	int sockfd = -1;
	int flag_reuse = 1;

	if (pipe2(pipefd, O_CLOEXEC | O_NONBLOCK) == -1) {
		ast_log(LOG_ERROR, "server_init failed: pipe2: %s\n", strerror(errno));
		goto error;
	}

	sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (sockfd == -1) {
		ast_log(LOG_ERROR, "server_init failed: socket: %s\n", strerror(errno));
		goto error;
	}

	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &flag_reuse, sizeof(flag_reuse)) == -1) {
		ast_log(LOG_WARNING, "server_init: setsockopt REUSEADDR: %s\n", strerror(errno));
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(3333);
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	if (bind(sockfd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
		ast_log(LOG_ERROR, "server_init failed: bind: %d %s\n", errno, strerror(errno));
		goto error;
	}

	s->pipefd[0] = pipefd[0];
	s->pipefd[1] = pipefd[1];
	s->sockfd = sockfd;

	return 0;

error:
	if (pipefd[0] != -1) {
		close(pipefd[0]);
		close(pipefd[1]);
	}

	if (sockfd != -1) {
		close(sockfd);
	}

	return -1;
}

static void server_destroy(struct server *s)
{
	close(s->pipefd[0]);
	close(s->pipefd[1]);
	s->pipefd[0] = -1;
	s->pipefd[1] = -1;

	close(s->sockfd);
	s->sockfd = -1;
}

static void *server_run(void *data)
{
	struct server *s = data;
	struct sockaddr_in addr;
	struct pollfd fds[2];
	char buf[INET_ADDRSTRLEN];
	int nfds;
	int fd;
	socklen_t addrlen;

	if (listen(s->sockfd, 10) == -1) {
		ast_log(LOG_ERROR, "server_run failed: listen: %s\n", strerror(errno));
		goto end;
	}

	fds[0].fd = s->sockfd;
	fds[0].events = POLLIN | POLLPRI;
	fds[1].fd = s->pipefd[0];
	fds[1].events = POLLIN;

	for (;;) {
		nfds = poll(fds, ARRAY_LEN(fds), 5000);
		switch (nfds) {
		case -1:
			ast_log(LOG_ERROR, "server_run failed: poll: %s\n", strerror(errno));
			goto end;
		case 0:
			ast_log(LOG_DEBUG, "server_run: poll timeout\n");
			continue;
		}

		if (fds[1].revents) {
			read(s->pipefd[0], buf, sizeof(buf));
			goto end;
		}

		if (fds[0].revents) {
			addrlen = sizeof(addr);
			fd = accept(s->sockfd, (struct sockaddr *) &addr, &addrlen);
			if (fd == -1) {
				ast_log(LOG_ERROR, "server_run: accept: %s\n", strerror(errno));
				continue;
			}

			memset(buf, 0, sizeof(buf));
			inet_ntop(AF_INET, &addr.sin_addr, buf, sizeof(buf));
			ast_verbose("New connection accepted from %s:%d\n", buf, ntohs(addr.sin_port));

			if (send(fd, buf, sizeof(buf), 0) == -1) {
				ast_log(LOG_WARNING, "server_run: send: %s\n", strerror(errno));
			}

			close(fd);
		}
	}

end:
	ast_log(LOG_NOTICE, "server_run: leaving\n");

	return NULL;
}

static int server_start(struct server *s)
{
	int ret;

	ret = ast_pthread_create(&s->thread, NULL, server_run, s);
	if (ret) {
		ast_log(LOG_ERROR, "server_start failed: pthread create: %s\n", strerror(ret));
		return -1;
	}

	return 0;
}

static int server_stop(struct server *s)
{
	static const uint32_t val = 1;
	ssize_t n;
	int ret;

	n = write(s->pipefd[1], &val, sizeof(val));
	if (n != sizeof(val)) {
		ast_log(LOG_ERROR, "server_stop failed: write returned %d\n", (int) n);
	}

	ret = pthread_join(s->thread, NULL);
	if (ret) {
		ast_log(LOG_ERROR, "server_stop failed: pthread join: %s\n", strerror(ret));
	}

	return 0;
}

static int load_module(void)
{
	if (server_init(&global_server) == -1) {
		return AST_MODULE_LOAD_DECLINE;
	}

	if (server_start(&global_server) == -1) {
		server_destroy(&global_server);
		return AST_MODULE_LOAD_DECLINE;
	}

	return AST_MODULE_LOAD_SUCCESS;
}

static int unload_module(void)
{
	server_stop(&global_server);
	server_destroy(&global_server);

	return 0;
}

AST_MODULE_INFO(ASTERISK_GPL_KEY, AST_MODFLAG_DEFAULT, "Lab Module",
	.load = load_module,
	.unload = unload_module,
);
