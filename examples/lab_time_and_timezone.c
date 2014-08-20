#include <asterisk.h>
#include <asterisk/cli.h>
#include <asterisk/localtime.h>
#include <asterisk/module.h>
#include <asterisk/time.h>

static char *cli_time(struct ast_cli_entry *e, int cmd, struct ast_cli_args *a)
{
	struct timeval date_tv;
	struct ast_tm tm = { 0, };
	const char *zone = NULL;
	const char *format = "%Y-%m-%d %H:%M:%S.%q";
	char buf[64];

	switch (cmd) {
	case CLI_INIT:
		e->command = "lab time";
		e->usage = "lab time <timezone> <format>\n";
		return NULL;
	case CLI_GENERATE:
		return NULL;
	}

	if (a->argc >= 3) {
		zone = a->argv[2];
	}

	if (a->argc >= 4) {
		format = a->argv[3];
	}

	if (a->argc >= 5) {
		return CLI_SHOWUSAGE;
	}

	date_tv = ast_tvnow();
	ast_localtime(&date_tv, &tm, zone);
	tm.tm_usec = 123;
	ast_strftime(buf, sizeof(buf), format, &tm);

	ast_cli(a->fd, "%s\nus: %d\n", buf, tm.tm_usec);

	return CLI_SUCCESS;
}

static struct ast_cli_entry cli_entries[] = {
	AST_CLI_DEFINE(cli_time, ""),
};

static int load_module(void)
{
	ast_cli_register_multiple(cli_entries, ARRAY_LEN(cli_entries));

	return AST_MODULE_LOAD_SUCCESS;
}

static int unload_module(void)
{
	ast_cli_unregister_multiple(cli_entries, ARRAY_LEN(cli_entries));

	return 0;
}

AST_MODULE_INFO(ASTERISK_GPL_KEY, AST_MODFLAG_DEFAULT, "Lab Module",
	.load = load_module,
	.unload = unload_module,
);
