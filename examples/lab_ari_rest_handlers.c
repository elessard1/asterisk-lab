#include <asterisk.h>
#include <asterisk/ari.h>
#include <asterisk/json.h>
#include <asterisk/module.h>

/*
 * curl -u 'xivo:Nasheow8Eag' http://127.0.0.1:5039/ari/lab/bar
 */
static void ari_lab_bar_cb(
	struct ast_tcptls_session_instance *ser,
	struct ast_variable *get_params, struct ast_variable *path_vars,
	struct ast_variable *headers, struct ast_ari_response *response)
{
	struct ast_json *json;

	json = ast_json_pack("{s: s}", "hello", "world");
	if (!json) {
		ast_ari_response_alloc_failed(response);
		return;
	}

	ast_ari_response_ok(response, json);
}

static void ari_lab_foo_cb(
	struct ast_tcptls_session_instance *ser,
	struct ast_variable *get_params, struct ast_variable *path_vars,
	struct ast_variable *headers, struct ast_ari_response *response)
{
	/* TODO complete */
	ast_ari_response_no_content(response);
}

static struct stasis_rest_handlers lab_bar = {
	.path_segment = "bar",
	.callbacks = {
		[AST_HTTP_GET] = ari_lab_bar_cb,
	},
	.num_children = 0,
	.children = {  }
};

static struct stasis_rest_handlers lab_foo = {
	.path_segment = "foo",
	.callbacks = {
		[AST_HTTP_POST] = ari_lab_foo_cb,
	},
	.num_children = 0,
	.children = {  }
};

static struct stasis_rest_handlers lab = {
	.path_segment = "lab",
	.callbacks = {
	},
	.num_children = 2,
	.children = { &lab_foo,&lab_bar, }
};

static int load_module(void)
{
	if (ast_ari_add_handler(&lab)) {
		return AST_MODULE_LOAD_DECLINE;
	}

	return AST_MODULE_LOAD_SUCCESS;
}

static int unload_module(void)
{
	ast_ari_remove_handler(&lab);

	return 0;
}

AST_MODULE_INFO(ASTERISK_GPL_KEY, AST_MODFLAG_DEFAULT, "Lab Module",
	.load = load_module,
	.unload = unload_module,
	.nonoptreq = "res_ari",
);
