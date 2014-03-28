#include <asterisk.h>
#include <asterisk/cli.h>
#include <asterisk/module.h>

static pthread_t monitor_thread;
static int pipefd[2] = {-1, -1};

static void *monitor_thread_loop(void __attribute__((unused)) *data)
{
	char *buf[256];
	ssize_t res;

	while (1) {
		res = read(pipefd[0], buf, 16);
		if (res == -1) {
			ast_log(LOG_NOTICE, "Pipe read returned -1...\n");
			break;
		} else if (res == 0) {
			ast_log(LOG_NOTICE, "Pipe closed... or something\n");
			break;
		}

		ast_log(LOG_NOTICE, "Read %d bytes from pipe\n", (int) res);
	}

	ast_log(LOG_NOTICE, "Leaving monitor thread\n");

	return NULL;
}

static int start_monitor_thread(void)
{
	return ast_pthread_create_background(&monitor_thread, NULL, monitor_thread_loop, NULL);
}

static void stop_monitor_thread(void)
{
	if (pthread_cancel(monitor_thread)) {
		ast_log(LOG_WARNING, "Error while calling pthread_cancel\n");
		return;
	}

	pthread_join(monitor_thread, NULL);
}

static void close_pipe_fd(int i)
{
	if (pipefd[i] == -1) {
		return;
	}

	close(pipefd[i]);
	pipefd[i] = -1;
}

static void close_pipe(void)
{
	int i;

	for (i = 0; i < 2; i++) {
		close_pipe_fd(i);
	}
}

static char *lab_write_to_pipe(struct ast_cli_entry *e, int cmd, struct ast_cli_args *a)
{
	static const int foo = 0x42;
	ssize_t res;

	switch (cmd) {
	case CLI_INIT:
		e->command = "lab writepipe";
		e->usage = "Usage: lab writepipe\n";
		return NULL;
	case CLI_GENERATE:
		return NULL;
	}

	res = write(pipefd[1], &foo, sizeof(foo));

	ast_cli(a->fd, "Wrote %d bytes to pipe\n", (int) res);

	return CLI_SUCCESS;
}

static char *lab_close_write_pipe(struct ast_cli_entry *e, int cmd, struct ast_cli_args *a)
{
	static const int i = 1;

	switch (cmd) {
	case CLI_INIT:
		e->command = "lab closewritepipe";
		e->usage = "Usage: lab closewritepipe\n";
		return NULL;
	case CLI_GENERATE:
		return NULL;
	}

	if (pipefd[i] == -1) {
		ast_cli(a->fd, "Write pipe is already closed\n");
		return CLI_SUCCESS;
	}

	close_pipe_fd(i);

	return CLI_SUCCESS;
}

static struct ast_cli_entry cli_test[] = {
		AST_CLI_DEFINE(lab_write_to_pipe, "Write something to the pipe"),
		AST_CLI_DEFINE(lab_close_write_pipe, "Close write pipe"),
};

static int load_module(void)
{
	if (pipe(pipefd) == -1) {
		ast_log(LOG_ERROR, "Could not allocate pipe\n");
		goto failure;
	}

	if (start_monitor_thread()) {
		ast_log(LOG_ERROR, "Could not start monitor thread\n");
		goto failure;
	}

	ast_cli_register_multiple(cli_test, ARRAY_LEN(cli_test));

	return AST_MODULE_LOAD_SUCCESS;

failure:
	close_pipe();

	return AST_MODULE_LOAD_DECLINE;
}

static int unload_module(void)
{
	ast_cli_unregister_multiple(cli_test, ARRAY_LEN(cli_test));
	stop_monitor_thread();
	close_pipe();

	return 0;
}

AST_MODULE_INFO(ASTERISK_GPL_KEY, AST_MODFLAG_DEFAULT, "Lab Module",
	.load = load_module,
	.unload = unload_module,
);
