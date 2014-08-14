#include <asterisk.h>
#include <asterisk/channel.h>
#include <asterisk/channelstate.h>
#include <asterisk/cli.h>
#include <asterisk/module.h>

static char *cli_channel_up(struct ast_cli_entry *e, int cmd, struct ast_cli_args *a)
{
	struct ast_channel *chan;

	switch (cmd) {
	case CLI_INIT:
		e->command = "channel up";
		e->usage = "channel up <channel>";
		return NULL;
	case CLI_GENERATE:
		return NULL;
	}

	if (a->argc != 3) {
		return CLI_FAILURE;
	}

	chan = ast_channel_get_by_name(a->argv[2]);
	if (!chan) {
		return CLI_FAILURE;
	}

	ast_setstate(chan, AST_STATE_UP);

	ast_channel_unref(chan);

	return CLI_SUCCESS;
}

static struct ast_cli_entry cli_entries[] = {
	AST_CLI_DEFINE(cli_channel_up, ""),
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
