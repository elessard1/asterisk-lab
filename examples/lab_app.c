#include <asterisk.h>
#include <asterisk/channel.h>
#include <asterisk/frame.h>
#include <asterisk/module.h>

static const char *app = "IndicateHold";

static int app_exec(struct ast_channel *chan, const char *data)
{
	ast_indicate(chan, AST_CONTROL_HOLD);

	return 0;
}

static int load_module(void) {
	if (ast_register_application(app, app_exec, "", "")) {
		return AST_MODULE_LOAD_DECLINE;
	}

	return AST_MODULE_LOAD_SUCCESS;
}

static int unload_module(void) {
	ast_unregister_application(app);

	return 0;
}

AST_MODULE_INFO(ASTERISK_GPL_KEY, AST_MODFLAG_DEFAULT, "Lab Module",
	.load = load_module,
	.unload = unload_module,
);
