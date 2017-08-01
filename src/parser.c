#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "format.h"
#include "parser.h"
#include "log.h"
#include "alloc.h"

Config_module_t *ConfigModules;

/*
 * free all tags.
 */
void free_tags(Tag_t *tag)
{
	Tag_t *tmp;

	if (!tag) return;
	while (tag) {
		tmp = tag->next;
		if (tag->data) {
			free(tag->data);
		}
		free(tag);

		tag = tmp;
	}
}

/*
 * free all actions.
 */
void free_actions(Action_t *action)
{
	Action_t *tmp;

	if (!action) return;
	while (action) {
		tmp = action->next;
		if (action->name) {
			free(action->name);
		}
		free(action);

		action = tmp;
	}
}

/*
 * free one config.
 */
void free_config(Config_t *config)
{
	if (!config) return;
	
	free_actions(config->action);
	free(config);
}

/*
 * free config modules.
 */
void free_config_modules(Config_module_t *mod)
{
	Config_module_t *tmp;

	if (!mod) {
		mod = ConfigModules;
	}

	while (mod) {
		tmp = mod->next;
		if (mod->config_name) {
			free(mod->config_name);
		}
		free(mod);

		mod = tmp;
	}
}

static Action_t *extract_max_action(Action_t **first)
{
	u32 max = 0;
	Action_t *tmp, *prev;

	// check first action memory.
	if (!first || !*first) {
		return NULL;
	}

	// find the max number action.
	tmp = *first;
	while (tmp) {
		if (tmp->no >= max) {
			max = tmp->no;
		}
		tmp = tmp->next;
	}

	// extract the max number action.
	prev = NULL;
	tmp = *first;
	while (tmp) {
		if (tmp->no == max) {
			if (!prev) {
				*first = tmp->next;
			} else {
				prev->next = tmp->next;
			}
			return tmp;
		}
		prev = tmp;
		tmp = tmp->next;
	}
	return NULL;
}

static void sort_actions(Action_t **action)
{
	Action_t *first;
	Action_t *order, *extract;

	if (!action || !*action) {
		return;
	}
	first = *action;

	order = NULL;
	while ((extract = extract_max_action(&first))) {
		if (!order) {
			extract->next = NULL;
			order = extract;
		} else {
			extract->next = order;
			order = extract;
		}
	}

	*action = order;
}

/*
 * load config by module name.
 */
Config_t *do_parser(const char *config_name, char *args)
{
	Config_module_t *idx;
	Config_operations_t *op;
	Config_t *config;
	Action_t *action;

	idx = ConfigModules;
	while (idx) {
		if (!strcasecmp(idx->config_name, config_name)) {
			op = &idx->op;
			if (!op->do_parser) {
				return NULL;
			}
			config = op->do_parser(args);
			if (!config) {
				return NULL;
			}

			sort_actions(&config->action);

			// allocate action's config to this config.
			action = config->action;
			while (action) {
				action->config = config;
			}
			return config;
		}
		idx = idx->next;
	}
	return NULL;
}

void init_parser(void)
{
	Config_module_t *idx;

	idx = ConfigModules;
	while (idx) {
		idx->op.init_parser();
		idx = idx->next;
	}
}

void done_parser(void)
{
	Config_module_t *idx;

	idx = ConfigModules;
	while (idx) {
		idx->op.done_parser();
		idx = idx->next;
	}
}

void register_parser_module(const char *parser_name, Config_operations_t *op)
{
	Config_module_t *idx;

	idx = ConfigModules;
	if (!idx) {
		ConfigModules = alloc_sizeof(Config_module_t);
		ConfigModules->config_name = strdup(parser_name);
		ConfigModules->op.init_parser = op->init_parser;
		ConfigModules->op.do_parser = op->do_parser;
		ConfigModules->op.done_parser = op->done_parser;
	}
	else {
		while (idx->next) {
			if (!strcasecmp(idx->config_name, parser_name)) {
				echo.f("Duplicate config module. %s", parser_name);
			}
			idx = idx->next;
		}

		idx->next = alloc_sizeof(Config_module_t);
		idx->config_name = strdup(parser_name);
		idx->op.init_parser = op->init_parser;
		idx->op.do_parser = op->do_parser;
		idx->op.done_parser = op->done_parser;
	}
	echo.d("register parser module [%s]", parser_name);
}

void debug_action(Action_t *action)
{
	echo.i("no = %d", action->no);
	echo.i("action = %s", action->name);
	echo.i("channel = %d", action->channel);
	echo.i("dwell = %d", action->dwell);

	if (cv_enabled(action->type)) {
		echo.i("type = %d", action->type);
	}
	if (cv_enabled(action->subtype)) {
		echo.i("subtype = %d", action->subtype);
	}
	if (cv_enabled(action->tods)) {
		echo.i("tods = %d", action->tods);
	}
	if (cv_enabled(action->fromds)) {
		echo.i("fromds = %d", action->fromds);
	}
	if (cv_enabled(action->addr_count)) {
		echo.i("addr_count = %d", action->addr_count);
		switch (action->addr_count) {
		case 1:
			echo.i("addr1 = "_MAC_FMT_, _MAC_FMT_FILL_(action->addr1));
		case 2:
			echo.i("addr2 = "_MAC_FMT_, _MAC_FMT_FILL_(action->addr2));
		case 3:
			echo.i("addr3 = "_MAC_FMT_, _MAC_FMT_FILL_(action->addr3));
		case 4:
			echo.i("addr4 = "_MAC_FMT_, _MAC_FMT_FILL_(action->addr4));
			break;
		}
	}
	else {
		if (cv_enabled(action->any_addr)) {
			echo.i("any_addr = "_MAC_FMT_, _MAC_FMT_FILL_(action->any_addr));
		}
		else {
			if (cv_enabled(action->ap_addr)) {
				echo.i("ap_addr = "_MAC_FMT_, _MAC_FMT_FILL_(action->ap_addr));
			}
			if (cv_enabled(action->st_addr)) {
				echo.i("st_addr = "_MAC_FMT_, _MAC_FMT_FILL_(action->st_addr));
			}
		}
	}

}

void debug_config(Config_t *config)
{
	Action_t *action;

	if (!config) {
		return;
	}

	echo.i("version = %u\n", config->version);
	action = config->action;
	while (action) {
		debug_action(action);
		action = action->next;
	}
}

Action_t *max_dwell_action(Config_t *config)
{
	u32 max = 0;
	Action_t *action;
	if (!config) {
		return NULL;
	}

	action = config->action;
	while (action) {
		if (max < action->dwell) {
			max = action->dwell;
		}
		action = action->next;
	}

	action = config->action;
	while (action) {
		if (max == action->dwell) {
			return action;
		}
		action = action->next;
	}
	return NULL;
}




extern void setup_xml_parser_module(void);

void setup_parser_modules(void)
{
	setup_xml_parser_module();
//	init_mysql_config();
//	init_sqlite_config();
}

