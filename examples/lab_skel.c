#include <asterisk.h>
#include <asterisk/cli.h>
#include <asterisk/module.h>

static char *lab_foo(struct ast_cli_entry *e, int cmd, struct ast_cli_args *a)
{
	switch (cmd) {
	case CLI_INIT:
		e->command = "lab foo";
		return NULL;
	case CLI_GENERATE:
		return NULL;
	}

	ast_cli(a->fd, "Foo\n");

	return CLI_SUCCESS;
}

static struct ast_cli_entry cli_test[] = {
	AST_CLI_DEFINE(lab_foo, ""),
};

static int load_module(void)
{
	ast_cli_register_multiple(cli_test, ARRAY_LEN(cli_test));

	ast_log(LOG_NOTICE, "Lab module loaded\n");

	return AST_MODULE_LOAD_SUCCESS;
}

static int unload_module(void)
{
	ast_cli_unregister_multiple(cli_test, ARRAY_LEN(cli_test));

	return 0;
}

AST_MODULE_INFO(ASTERISK_GPL_KEY, AST_MODFLAG_DEFAULT, "Lab Module",
	.load = load_module,
	.unload = unload_module,
);
