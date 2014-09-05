#include <asterisk.h>
#include <asterisk/cli.h>
#include <asterisk/module.h>
#include <asterisk/stringfields.h>

struct test {
	AST_DECLARE_STRING_FIELDS(
		AST_STRING_FIELD(f1);
	);
};

static char data[64];
static struct test test;

#define log_number_of_pool(p) log_number_of_pool_((p)->__field_mgr_pool)

static void log_number_of_pool_(struct ast_string_field_pool *pool)
{
	size_t total_size = 0;
	size_t total_used = 0;
	size_t total_active = 0;
	unsigned int count;

	for (count = 0; pool; count++, pool = pool->prev) {
		total_size += pool->size;
		total_used += pool->used;
		total_active += pool->active;
	}

	ast_log(LOG_NOTICE, "pool stats (count, size, used, active): %u %zu %zu %zu\n", count, total_size, total_used, total_active);
}

static char *cli_set(struct ast_cli_entry *e, int cmd, struct ast_cli_args __attribute__((unused)) *a)
{
	switch (cmd) {
	case CLI_INIT:
		e->command = "lab set";
		return NULL;
	case CLI_GENERATE:
		return NULL;
	}

	ast_string_field_set(&test, f1, "");
	ast_string_field_set(&test, f1, data);
	log_number_of_pool(&test);

	return CLI_SUCCESS;
}

static struct ast_cli_entry cli_entries[] = {
	AST_CLI_DEFINE(cli_set, ""),
};

static int load_module(void)
{
	memset(data, 'F', sizeof(data) - 1);
	ast_string_field_init(&test, 128);

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
