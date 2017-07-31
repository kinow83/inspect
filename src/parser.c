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
		if (action->action_name) {
			free(action->action_name);
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
	uint32_t max = 0;
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
Config_t *load_config(const char *config_name, const char *args)
{
	Config_module_t *idx;
	Config_t *config;
	Action_t *action;

	idx = ConfigModules;
	while (idx) {
		if (!strcasecmp(idx->config_name, config_name)) {
			if (!idx->load) {
				return NULL;
			}
			config = idx->load(args, idx->context);

			sort_actions(&config->action);

			// allocate action's config to this config.
			action = config->action;
			while (action) {
				action->config = config;
			}

			debug_config(config);
			return config;
		}
	}
	return NULL;
}

void register_config_module(const char *config_name, Load_func_t load, void *context)
{
	Config_module_t *idx;

	idx = ConfigModules;
	if (!idx) {
		ConfigModules = alloc1(Config_module_t);
		ConfigModules->config_name = strdup(config_name);
		ConfigModules->load = load;
		ConfigModules->context = context;
	}
	else {
		while (idx->next) {
			if (!strcasecmp(idx->config_name, config_name)) {
				echo.f("Duplicate config module. %s", config_name);
			}
			idx = idx->next;
		}

		idx->next = alloc1(Config_module_t);
		idx->config_name = strdup(config_name);
		idx->load = load;
		idx->context = context;
	}
}

void debug_action(Action_t *action)
{
	echo.i("no = %d", action->no);
	echo.i("action_name = %s", action->action_name);
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
	uint32_t max = 0;
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




extern void init_xml_config_module();

void init_config_modules()
{
	init_xml_config_module();
//	init_mysql_config();
//	init_sqlite_config();
}

