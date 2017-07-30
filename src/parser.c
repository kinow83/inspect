#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "format.h"
#include "parser.h"
#include "log.h"

config_list_t *ConfigList;

static void free_action(action_t *action) {
	if (!action) return;
	if (action->config_type) {
		free(action->config_type);
	}
}

void free_config(config_t *config) {
	action_t *tmpa, *action;

	if (!config) return;

	action = config->action;
	while (action) {
		tmpa = action->next;
		free_action(action);
		free(action);
		action = tmpa;
	}

	free(config);
}

void register_config(const char *config_type, config_operation_t *config_op) {
	config_list_t *idx;

	idx = ConfigList;
	if (!idx) {
		ConfigList = (config_list_t *)calloc(1, sizeof(config_list_t));
		ConfigList->entry.config_type = strdup(config_type);
		ConfigList->entry.config_load = config_op->config_load;
		ConfigList->entry.config_debug = config_op->config_debug;
	}
	else {
		while (idx->next) {
			if (!strcasecmp(idx->entry.config_type, config_type)) {
				echo.f("Duplicate config module. %s", config_type);
			}
			idx = idx->next;
		}

		idx->next = (config_list_t *)calloc(1, sizeof(config_list_t *));
		idx->entry.config_type = strdup(config_type);
		idx->entry.config_load = config_op->config_load;
		idx->entry.config_debug = config_op->config_debug;
	}
}

void init_config() {
	init_xml_config();
}
