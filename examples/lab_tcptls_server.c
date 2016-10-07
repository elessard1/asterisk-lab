#include <asterisk.h>
#include <asterisk/module.h>
#include <asterisk/tcptls.h>

#include <fcntl.h>

static void foo_periodic_fn(void *data)
{
	ast_debug(1, "in foo periodic fn\n");
}

static void *session_do(void *data)
{
	static const int flag_nodelay = 1;
	static const char *msg = "hello world\n";
	struct ast_tcptls_session_instance *ser = data;
	int flags;
	int res;

	ast_debug(1, "session_do called (new connection)\n");

	/* manager.c / session_do() */
	if (setsockopt(ser->fd, IPPROTO_TCP, TCP_NODELAY, &flag_nodelay, sizeof(flag_nodelay)) == -1) {
		ast_log(LOG_WARNING, "session_do: setsockopt TCP_NODELAY failed: %s\n", strerror(errno));
	}

	/* manager.c / session_do() */
	flags = fcntl(ser->fd, F_GETFL);
	flags |= O_NONBLOCK;
	if (fcntl(ser->fd, F_SETFL, flags) == -1) {
		ast_log(LOG_WARNING, "session_do: fcntl failed: %s\n", strerror(errno));
	}

	res = ast_careful_fwrite(ser->f, ser->fd, msg, strlen(msg), 100);
	if (res) {
		ast_log(LOG_WARNING, "session_do: ast_careful_fwrite returned %d\n", res);
	}

	res = ast_wait_for_input(ser->fd, 30000);
	if (res == -1) {
		ast_log(LOG_WARNING, "session_do: ast_wait_for_input returned %d\n", res);
	}

	/* manager.c / session_destructor() */
	fflush(ser->f);
	fclose(ser->f);

	ao2_ref(ser, -1);

	return NULL;
}

static struct ast_tcptls_session_args server_desc = {
	.accept_fd = -1,
	.master = AST_PTHREADT_NULL,
	.tls_cfg = NULL,
	.poll_timeout = 5000,
	.periodic_fn = foo_periodic_fn,
	.name = "Lab server",
	.accept_fn = ast_tcptls_server_root,	/* thread doing the accept() */
	.worker_fn = session_do,	/* thread handling the session */
};

static int load_module(void)
{
	ast_sockaddr_setnull(&server_desc.local_address);
	ast_sockaddr_parse(&server_desc.local_address, "0.0.0.0", 0);
	ast_sockaddr_set_port(&server_desc.local_address, 4444);

	ast_tcptls_server_start(&server_desc);

	return AST_MODULE_LOAD_SUCCESS;
}

static int unload_module(void)
{
	ast_tcptls_server_stop(&server_desc);

	return 0;
}

AST_MODULE_INFO(ASTERISK_GPL_KEY, AST_MODFLAG_DEFAULT, "Lab Module",
	.load = load_module,
	.unload = unload_module,
);
