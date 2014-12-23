#include <asterisk.h>
#include <asterisk/devicestate.h>
#include <asterisk/event.h>
#include <asterisk/module.h>

static struct ast_event_sub *device_state_sub;

static void device_state_cb(const struct ast_event *event, void __attribute__((unused)) *unused)
{
	const char *device = ast_event_get_ie_str(event, AST_EVENT_IE_DEVICE);
	enum ast_device_state state = ast_event_get_ie_uint(event, AST_EVENT_IE_STATE);

	if (ast_strlen_zero(device)) {
		ast_log(LOG_ERROR, "Received invalid event that had no device IE\n");
		return;
	}

	ast_log(LOG_NOTICE, "Device %s state is %s\n", device, ast_devstate2str(state));
}

static int load_module(void)
{
	device_state_sub = ast_event_subscribe(AST_EVENT_DEVICE_STATE, device_state_cb, "Dump Device State", NULL, AST_EVENT_IE_END);
	if (!device_state_sub) {
		return AST_MODULE_LOAD_DECLINE;
	}

	return AST_MODULE_LOAD_SUCCESS;
}

static int unload_module(void)
{
	ast_event_unsubscribe(device_state_sub);

	return 0;
}

AST_MODULE_INFO(ASTERISK_GPL_KEY, AST_MODFLAG_DEFAULT, "Devstate Log Module",
	.load = load_module,
	.unload = unload_module,
	.load_pri = AST_MODPRI_DEVSTATE_CONSUMER,
);
