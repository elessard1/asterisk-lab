#include <asterisk.h>
#include <asterisk/cel.h>
#include <asterisk/cli.h>
#include <asterisk/event.h>
#include <asterisk/module.h>

static int report_dummy_cel_event(unsigned int us)
{
	struct ast_event *ev;

	ev = ast_event_new(AST_EVENT_CEL,
		AST_EVENT_IE_CEL_EVENT_TYPE, AST_EVENT_IE_PLTYPE_UINT, AST_CEL_APP_START,
		AST_EVENT_IE_CEL_EVENT_TIME, AST_EVENT_IE_PLTYPE_UINT, 1430309985,
		AST_EVENT_IE_CEL_EVENT_TIME_USEC, AST_EVENT_IE_PLTYPE_UINT, us,
		AST_EVENT_IE_CEL_USEREVENT_NAME, AST_EVENT_IE_PLTYPE_STR, "",
		AST_EVENT_IE_CEL_CIDNAME, AST_EVENT_IE_PLTYPE_STR, "",
		AST_EVENT_IE_CEL_CIDNUM, AST_EVENT_IE_PLTYPE_STR, "",
		AST_EVENT_IE_CEL_CIDANI, AST_EVENT_IE_PLTYPE_STR, "",
		AST_EVENT_IE_CEL_CIDRDNIS, AST_EVENT_IE_PLTYPE_STR, "",
		AST_EVENT_IE_CEL_CIDDNID, AST_EVENT_IE_PLTYPE_STR, "",
		AST_EVENT_IE_CEL_EXTEN, AST_EVENT_IE_PLTYPE_STR, "",
		AST_EVENT_IE_CEL_CONTEXT, AST_EVENT_IE_PLTYPE_STR, "",
		AST_EVENT_IE_CEL_CHANNAME, AST_EVENT_IE_PLTYPE_STR, "Lab/test-event",
		AST_EVENT_IE_CEL_APPNAME, AST_EVENT_IE_PLTYPE_STR, "",
		AST_EVENT_IE_CEL_APPDATA, AST_EVENT_IE_PLTYPE_STR, "",
		AST_EVENT_IE_CEL_AMAFLAGS, AST_EVENT_IE_PLTYPE_UINT, 0,
		AST_EVENT_IE_CEL_ACCTCODE, AST_EVENT_IE_PLTYPE_STR, "",
		AST_EVENT_IE_CEL_PEERACCT, AST_EVENT_IE_PLTYPE_STR, "",
		AST_EVENT_IE_CEL_UNIQUEID, AST_EVENT_IE_PLTYPE_STR, "",
		AST_EVENT_IE_CEL_LINKEDID, AST_EVENT_IE_PLTYPE_STR, "",
		AST_EVENT_IE_CEL_USERFIELD, AST_EVENT_IE_PLTYPE_STR, "",
		AST_EVENT_IE_CEL_EXTRA, AST_EVENT_IE_PLTYPE_STR, "",
		AST_EVENT_IE_CEL_PEER, AST_EVENT_IE_PLTYPE_STR, "",
		AST_EVENT_IE_END);

	if (!ev) {
		return -1;
	}

	if (ast_event_queue(ev)) {
		ast_event_destroy(ev);
		return -1;
	}

	return 0;
}

static char *cli_cel(struct ast_cli_entry *e, int cmd, struct ast_cli_args *a)
{
	unsigned int us = 1;

	switch (cmd) {
	case CLI_INIT:
		e->command = "lab cel";
		e->usage = "lab cel <us>\n";
		return NULL;
	case CLI_GENERATE:
		return NULL;
	}

	if (a->argc >= 3) {
		us = (unsigned int) strtol(a->argv[2], NULL, 10);
	}

	if (!report_dummy_cel_event(us)) {
		ast_cli(a->fd, "Success.\n");
	} else {
		ast_cli(a->fd, "Fail.\n");
	}

	return CLI_SUCCESS;
}

static struct ast_cli_entry cli_entries[] = {
	AST_CLI_DEFINE(cli_cel, ""),
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
