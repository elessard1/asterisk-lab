#include <asterisk.h>
#include <asterisk/cli.h>
#include <asterisk/callerid.h>
#include <asterisk/module.h>

static char *cli_callerid(struct ast_cli_entry *e, int cmd, struct ast_cli_args *a)
{
	char buf[64] = "\"*10\"";
	char name_buf[64] = "";
	char location_buf[64] = "";
	char *name = name_buf;
	char *location = location_buf;

	switch (cmd) {
	case CLI_INIT:
		e->command = "lab callerid";
		return NULL;
	case CLI_GENERATE:
		return NULL;
	}

	ast_cli(a->fd, "buf: %s\n", buf);
	if (!ast_callerid_parse(buf, &name, &location)) {
		ast_cli(a->fd, "buf: %s\n", buf);
		ast_cli(a->fd, "name: %s\n", name);
		ast_cli(a->fd, "location: %s\n", location);
	}

	return CLI_SUCCESS;
}

static struct ast_cli_entry cli_entries[] = {
	AST_CLI_DEFINE(cli_callerid, ""),
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
