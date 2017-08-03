/*
 * resource.c
 *
 *  Created on: 2017. 8. 2.
 *      Author: root
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "format.h"
#include "resource.h"
#include "log.h"
#include "alloc.h"
#include "strings.h"


size_t num_module_option(Module_option_t *mopt)
{
	size_t n = 0;

	while (mopt) {
		n++;
		mopt = mopt->next;
	}
	return n;
}

Module_option_t* new_module_option(const char *args)
{
	char **line, **chunk;
	int nline, nchunk;
	int i;
	Module_option_t *mopt = NULL, *tmp;

	if (!args || !strlen(args)) return NULL;

	line = new_splits(args, ";", &nline);
	if (!line || nline == 0) {
		echo.f("Invalid module option: %s", args);
	}

	for (i=0; i<nline; i++) {
		chunk = new_splits(line[i], ":", &nchunk);
		if (!chunk || nchunk == 0) {
			echo.f("Invalid module option: %s", line[i]);
		}
		if (!(nchunk == 1 || nchunk == 2)) {
			echo.f("Invalid module option: %s", line[i]);
		}
		if (!strlen(chunk[0])) {
			echo.f("Invalid module option 'empty name': %s", line[i]);
		}
		if (nchunk == 2 && !strlen(chunk[1])) {
			echo.f("Invalid module option 'empty option': %s", line[i]);
		}

		if (!mopt) {
			mopt = alloc_sizeof(Module_option_t);
			mopt->name = strdup(chunk[0]);
			if (nchunk == 2) {
				mopt->options = strdup(chunk[1]);
			}
		}
		else {
			tmp = mopt;
			while (tmp->next) {
				tmp = tmp->next;
			}

			tmp->next = alloc_sizeof(Module_option_t);

			tmp = tmp->next;

			tmp->name = strdup(chunk[0]);
			tmp->options = strdup(chunk[1]);
		}
		free_splits(chunk, nchunk);
	}
	free_splits(line, nline);

	return mopt;
}

void free_module_option(Module_option_t *mopt)
{
	Module_option_t *cur, *tmp;

	if (!mopt) return;

	cur = mopt;
	while (cur) {
		tmp = cur->next;
		if (cur->name) {
			free(cur->name);
		}
		if (cur->options) {
			free(cur->options);
		}
		free(cur);

		cur = tmp;
	}
}

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

void free_actions(Action_t *action)
{
	Action_t *tmp;

	if (!action) return;
	while (action) {
		tmp = action->next;
		if (action->name) {
			free(action->name);
		}
		if (action->tags) {
			free_tags(action->tags);
		}
		free(action);

		action = tmp;
	}
}

void free_config(Config_t *config)
{
	if (!config) return;
	free_actions(config->action);
	free(config);
}


static Action_t *extract_max_no_action(Action_t **first)
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

void sort_actions(Action_t **action)
{
	Action_t *first;
	Action_t *order, *extract;

	if (!action || !*action) {
		return;
	}
	first = *action;

	order = NULL;
	while ((extract = extract_max_no_action(&first))) {
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

void debug_action(Action_t *action)
{
	echo.D("[action debug] ---------------------------");
	echo.D("no = %d", action->no);
	echo.D("action = %s", action->name);
	echo.D("channel = %d", action->channel);
	echo.D("dwell = %d", action->dwell);

	if (cv_enabled(action->type)) {
		echo.D("type = %d", action->type);
	}
	if (cv_enabled(action->subtype)) {
		echo.D("subtype = %d", action->subtype);
	}
	if (cv_enabled(action->tods)) {
		echo.D("tods = %d", action->tods);
	}
	if (cv_enabled(action->fromds)) {
		echo.D("fromds = %d", action->fromds);
	}
	if (cv_enabled(action->addr_count)) {
		echo.D("addr_count = %d", action->addr_count);
		switch (action->addr_count) {
		case 1:
			echo.D("addr1 = "_MAC_FMT_, _MAC_FMT_FILL_(action->addr1));
		case 2:
			echo.D("addr2 = "_MAC_FMT_, _MAC_FMT_FILL_(action->addr2));
		case 3:
			echo.D("addr3 = "_MAC_FMT_, _MAC_FMT_FILL_(action->addr3));
		case 4:
			echo.D("addr4 = "_MAC_FMT_, _MAC_FMT_FILL_(action->addr4));
			break;
		}
	}
	else {
		if (cv_enabled(action->any_addr)) {
			echo.D("any_addr = "_MAC_FMT_, _MAC_FMT_FILL_(action->any_addr));
		}
		else {
			if (cv_enabled(action->ap_addr)) {
				echo.D("ap_addr = "_MAC_FMT_, _MAC_FMT_FILL_(action->ap_addr));
			}
			if (cv_enabled(action->st_addr)) {
				echo.D("st_addr = "_MAC_FMT_, _MAC_FMT_FILL_(action->st_addr));
			}
		}
	}
	echo.D("------------------------------------------");
}

void debug_config(Config_t *config)
{
	Action_t *action;

	if (!config) return;

	echo.D("[config debug] ==============================");
	echo.D("config = %s", config->name);
	echo.D("version = %u", config->version);
	action = config->action;
	while (action) {
		debug_action(action);
		action = action->next;
	}
	echo.D("==============================================");
}

Action_t *get_max_dwell(Config_t *config)
{
	u32 max = 0;
	Action_t *action;

	if (!config) return NULL;

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
