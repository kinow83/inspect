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

void free_action_details(Action_details_t *detail)
{
	Action_details_t *tmp;

	if (!detail) return;

	while (detail) {
		tmp = detail->next;
		if (detail->tags) {
			free_tags(detail->tags);
		}
		free(detail);

		detail = tmp;
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
		if (action->shooter) {
			free_action_details(action->shooter);
		}
		if (action->capture) {
			free_action_details(action->capture);
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
		if (tmp->id >= max) {
			max = tmp->id;
		}
		tmp = tmp->next;
	}

	// extract the max number action.
	prev = NULL;
	tmp = *first;
	while (tmp) {
		if (tmp->id == max) {
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

void debug_tags(Tag_t *tag)
{
	const char *type = "str";
	int len, n = 0, i;
	char *buf;

	if (tag->type == TAG_TYPE_HEX) {
		type = "hex";
		len = (tag->len * 2);
		buf = alloc_type(char*, len+1);
		for (i = 0; i < len; i++) {
			n += snprintf(buf + n, len - n, "%02x", tag->data[i]);
		}
	}
	else { // str
		len = tag->len;
		buf = alloc_type(char*, len+1);
		strncpy(buf, tag->data, len);
	}

	echo.D("\t\t\t id=%d, len=%d, type=%s, data=%s",
			tag->id, tag->len, type, buf);
}

void debug_action_details(Action_details_t *detail)
{
	Tag_t *tag;

	echo.D("\t\t id = %d", detail->id);

	if (cv_enabled(detail->type)) {
		echo.D("\t\t type = %d", detail->type);
	}
	if (cv_enabled(detail->subtype)) {
		echo.D("\t\t subtype = %d", detail->subtype);
	}
	if (cv_enabled(detail->tods)) {
		echo.D("\t\t tods = %d", detail->tods);
	}
	if (cv_enabled(detail->fromds)) {
		echo.D("\t\t fromds = %d", detail->fromds);
	}
	if (cv_enabled(detail->addr1)) {
		echo.D("\t\t addr1 = "_MAC_FMT_, _MAC_FMT_FILL_(detail->addr1));
	}
	if (cv_enabled(detail->addr2)) {
		echo.D("\t\t addr2 = "_MAC_FMT_, _MAC_FMT_FILL_(detail->addr2));
	}
	if (cv_enabled(detail->addr3)) {
		echo.D("\t\t addr2 = "_MAC_FMT_, _MAC_FMT_FILL_(detail->addr3));
	}
	if (cv_enabled(detail->addr4)) {
		echo.D("\t\t addr2 = "_MAC_FMT_, _MAC_FMT_FILL_(detail->addr4));
	}
	if (cv_enabled(detail->any_addr)) {
		echo.D("\t\t any_addr = "_MAC_FMT_, _MAC_FMT_FILL_(detail->any_addr));
	}
	if (cv_enabled(detail->ap_addr)) {
		echo.D("\t\t ap_addr = "_MAC_FMT_, _MAC_FMT_FILL_(detail->ap_addr));
	}
	if (cv_enabled(detail->st_addr)) {
		echo.D("\t\t st_addr = "_MAC_FMT_, _MAC_FMT_FILL_(detail->st_addr));
	}

	tag = detail->tags;
	while (tag) {
		debug_tags(tag);
		tag = tag->next;
	}
}

void debug_actions(Action_t *action)
{
	Action_details_t *detail;

	echo.D("\t [action] id = %d ---------------------------", action->id);
	echo.D("\t name = %s", action->name);
	echo.D("\t interval = %d", action->interval);
	echo.D("\t channel = %d", action->channel);

	detail = action->shooter;
	while (detail) {
		echo.D("\t [shooter]");
		debug_action_details(detail);
		detail = detail->next;
	}

	detail = action->capture;
	while (detail) {
		echo.D("\t [capture]");
		debug_action_details(detail);
		detail = detail->next;
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
		debug_actions(action);
		action = action->next;
	}
	echo.D("==============================================");
}

Action_t *get_max_interval(Config_t *config)
{
	u32 max = 0;
	Action_t *action;

	if (!config) return NULL;

	action = config->action;
	while (action) {
		if (max < action->interval) {
			max = action->interval;
		}
		action = action->next;
	}

	action = config->action;
	while (action) {
		if (max == action->interval) {
			return action;
		}
		action = action->next;
	}
	return NULL;
}
