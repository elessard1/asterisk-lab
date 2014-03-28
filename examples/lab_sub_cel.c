#include <asterisk.h>
#include <asterisk/cel.h>
#include <asterisk/event.h>
#include <asterisk/module.h>

static struct ast_event_sub *event_sub;

static void lab_log(const struct ast_event *event, void __attribute__((unused)) *userdata)
{
	struct ast_cel_event_record record = {
		.version = AST_CEL_EVENT_RECORD_VERSION,
	};

	if (ast_cel_fill_record(event, &record)) {
		return;
	}

	switch (record.event_type) {
	case AST_CEL_LINKEDID_END:
		ast_log(LOG_NOTICE, "received LINKEDID_END for channel %s\n", record.channel_name);
		break;
	default:
		break;
	}
}

static int load_module(void)
{
	event_sub = ast_event_subscribe(AST_EVENT_CEL, lab_log, "Lab Event Logging", NULL, AST_EVENT_IE_END);
	if (!event_sub) {
		ast_log(LOG_ERROR, "could not subscribe to CEL events\n");
	}

	return AST_MODULE_LOAD_SUCCESS;
}

static int unload_module(void)
{
	if (event_sub) {
		event_sub = ast_event_unsubscribe(event_sub);
	}

	return 0;
}

AST_MODULE_INFO(ASTERISK_GPL_KEY, AST_MODFLAG_DEFAULT, "Lab Module",
	.load = load_module,
	.unload = unload_module,
);
