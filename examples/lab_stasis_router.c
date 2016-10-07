#include <asterisk.h>
#include <asterisk/core_local.h>
#include <asterisk/json.h>
#include <asterisk/logger.h>
#include <asterisk/module.h>
#include <asterisk/stasis.h>
#include <asterisk/stasis_channels.h>
#include <asterisk/stasis_message_router.h>

static struct stasis_message_router *router;

static struct stasis_topic *topic;

static struct stasis_forward *channel_forwarder;

static void dial_cb(void *data, struct stasis_subscription *sub, struct stasis_message *message)
{
	struct ast_multi_channel_blob *blob = stasis_message_data(message);
	struct ast_channel_snapshot *caller = ast_multi_channel_blob_get_channel(blob, "caller");
	struct ast_channel_snapshot *peer = ast_multi_channel_blob_get_channel(blob, "peer");
	struct ast_channel_snapshot *forwarded = ast_multi_channel_blob_get_channel(blob, "forwarded");
	struct ast_json *json = ast_multi_channel_blob_get_json(blob);

	if (!caller) {
		ast_log(LOG_WARNING, "caller is null\n");
	} else {
		ast_log(LOG_WARNING, "caller is %s %s %s\n", peer->type, caller->name, caller->uniqueid);
	}

	if (!peer) {
		ast_log(LOG_WARNING, "peer is null\n");
	} else {
		ast_log(LOG_WARNING, "peer is %s %s %s\n", peer->type, peer->name, peer->uniqueid);
	}

	if (!forwarded) {
		ast_log(LOG_WARNING, "forwarded is null\n");
	} else {
		ast_log(LOG_WARNING, "forwarded is %s %s\n", forwarded->name, forwarded->uniqueid);
	}

	if (json) {
		char *s = ast_json_dump_string(json);

		ast_log(LOG_WARNING, "json: %s\n", s);
		ast_json_free(s);
	}
}

static void local_bridge_cb(void *data, struct stasis_subscription *sub, struct stasis_message *message)
{
	struct ast_multi_channel_blob *blob = stasis_message_data(message);
	struct ast_channel_snapshot *c1 = ast_multi_channel_blob_get_channel(blob, "1");
	struct ast_channel_snapshot *c2 = ast_multi_channel_blob_get_channel(blob, "2");

	if (!c1) {
		ast_log(LOG_WARNING, "c1 is null\n");
	} else {
		ast_log(LOG_WARNING, "c1 is %s %s\n", c1->name, c1->uniqueid);
	}

	if (!c2) {
		ast_log(LOG_WARNING, "c2 is null\n");
	} else {
		ast_log(LOG_WARNING, "c2 is %s %s\n", c2->name, c2->uniqueid);
	}
}

static int load_module(void)
{
	topic = stasis_topic_create("lab_topic");
	if (!topic) {
		ast_log(LOG_ERROR, "error creating topic\n");
		goto error;
	}

	channel_forwarder = stasis_forward_all(
		ast_channel_topic_all_cached(),
		topic);
	if (!channel_forwarder) {
		ast_log(LOG_ERROR, "error creating forwarder\n");
		goto error;
	}

	router = stasis_message_router_create(topic);
	if (!router) {
		ast_log(LOG_ERROR, "error creating router\n");
		goto error;
	}

	if (stasis_message_router_add(router,
		ast_channel_dial_type(),
		dial_cb,
		NULL)) {
		ast_log(LOG_ERROR, "error adding ast_channel_dial_type\n");
	}

	if (stasis_message_router_add(router,
		ast_local_bridge_type(),
		local_bridge_cb,
		NULL)) {
		ast_log(LOG_ERROR, "error adding ast_locaL_bridge_type\n");
	}

	return AST_MODULE_LOAD_SUCCESS;

error:
	/* FIXME cleanup code is missing */
	return AST_MODULE_LOAD_DECLINE;
}

static int unload_module(void)
{
	/* FIXME cleanup code is missing */

	return 0;
}

AST_MODULE_INFO(ASTERISK_GPL_KEY, AST_MODFLAG_DEFAULT, "Lab Module",
	.load = load_module,
	.unload = unload_module,
);
