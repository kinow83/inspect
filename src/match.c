#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "format.h"
#include "log.h"
#include "alloc.h"
#include "match.h"

static Match_module_t *MatchModules;

#if 0
Match_module_t *get_match_modules(void)
{
	return MatchModules;
}
#endif

void init_match_modules(Module_option_t *mopt)
{
	Match_module_t *idx;

	if (!mopt) return;

	while (mopt) {
		idx = MatchModules;

		while (idx) {
			if (!strcasecmp(mopt->name, idx->match_name)) {
				idx->op.init_match(mopt->options);
				idx->enable = true;
				echo.d("enable module: %s", mopt->name);
				break;
			}
			idx = idx->next;
		}
		mopt = mopt->next;
	}
}

void finish_match_modules(void)
{
	u32 toolongtime = 0;
	Match_module_t *idx;

	idx = MatchModules;
	while (idx) {
		while (idx->finished == false) {
			usleep(10);
			toolongtime++;
			if (toolongtime > 10000) {
				echo.E("waiting too long time for finish match modules: %s", idx->match_name);
				toolongtime = 0;
			}
		}

		idx->op.finish_match();
		idx = idx->next;
	}
}

void free_match_moduels(Match_module_t *mod)
{
	Match_module_t *idx;

	if (!mod) {
		mod = MatchModules;
	}
	while (mod) {
		idx = mod->next;
		if (mod->match_name) free(mod->match_name);
		free(mod);

		mod = idx;
	}
}

/*
 * Fast matching for all match modules.
 */
Action_t *do_match(Config_t *config, u8 *h80211, size_t h80211len, struct rx_info *ri)
{
	Match_module_t *idx;
	Action_t *matched;

	idx = MatchModules;
	while (idx) {
		if (idx->enable == true) {
			idx->finished = false;
			matched = idx->op.do_match(config, h80211, h80211len, ri);

			if (matched) {
				return matched;
			}
		}
		idx = idx->next;
	}
	return NULL;
}

Action_t *do_match_by_name(Config_t *config,
		const char *name, u8 *h80211, size_t h80211len, struct rx_info *ri)
{
	Match_module_t *idx;
	Action_t *matched;

	idx = MatchModules;
	while (idx) {
		if ((idx->enable == true) && !strcasecmp(name, idx->match_name)) {
			idx->finished = false;
			matched = idx->op.do_match(config, h80211, h80211len, ri);
			return matched;
		}
		idx = idx->next;
	}
	return NULL;
}

void register_match_module(const char *match_name, Match_operations_t *op)
{
	Match_module_t *idx;

	idx = MatchModules;
	if (!idx) {
		MatchModules = alloc_sizeof(Match_module_t);
		MatchModules->enable = false;
		MatchModules->finished = true;
		MatchModules->match_name = strdup(match_name);
		MatchModules->op.init_match = op->init_match;
		MatchModules->op.do_match = op->do_match;
		MatchModules->op.finish_match = op->finish_match;
		MatchModules->op.usage_match = op->usage_match;
	}
	else {
		while (idx->next) {
			if (!strcasecmp(idx->match_name, match_name)) {
				echo.f("Duplicate match module. %s", match_name);
			}
			idx = idx->next;
		}

		idx->next = alloc_sizeof(Match_module_t);

		idx = idx->next;

		idx->enable = false;
		idx->finished = true;
		idx->match_name = strdup(match_name);
		idx->op.init_match = op->init_match;
		idx->op.do_match = op->do_match;
		idx->op.finish_match = op->finish_match;
		idx->op.usage_match = op->usage_match;
	}
	echo.d("register match module [%s]", match_name);
}

void usage_match_module(void)
{
	Match_module_t *idx;

	idx = MatchModules;
	while (idx) {
		echo.out("\t\t[%s]", idx->match_name);
		echo.out("\t\t\t%s", idx->op.usage_match());
		idx = idx->next;
	}
}

int num_match_modules(void)
{
	Match_module_t *idx;
	int count = 0;

	idx = MatchModules;
	while (idx) {
		count++;
		idx = idx->next;
	}
	return count;
}

int num_enabled_match_modules(void)
{
	Match_module_t *idx;
	int count = 0;

	idx = MatchModules;
	while (idx) {
		if (idx->enable) {
			count++;
		}
		idx = idx->next;
	}
	return count;
}

void mark_finished_match_module(const char *match_name)
{
	Match_module_t *idx;

	idx = MatchModules;
	while (idx) {
		if (!strcasecmp(match_name, idx->match_name)) {
			idx->finished = true;
//			echo.I("mark_finished_match_module: %s", idx->match_name);
			break;
		}
		idx = idx->next;
	}
}


extern void setup_wifi_match_module();

void setup_match_modules(void)
{
	setup_wifi_match_module();
}
