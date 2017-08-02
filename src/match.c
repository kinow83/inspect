#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "format.h"
#include "log.h"
#include "alloc.h"
#include "match.h"

static Match_module_t *MatchModules;


void init_match_modules(Module_option_t **match_opts)
{
	Match_module_t *idx;
	Module_option_t *opt;
	int i = 0;

	if (!match_opts) return;

	while (match_opts[i]) {
		opt = match_opts[i];
		idx = MatchModules;

		while (idx) {
			if (!strcasecmp(opt->name, idx->match_name)) {
				idx->op.init_match(opt->options);
				idx->enable = true;
				break;
			}
			idx = idx->next;
		}
		i++;
	}
}

void finish_match_modules(void)
{
	Match_module_t *idx;

	idx = MatchModules;
	while (idx) {
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
		if (mod->match_name) {
			free(mod->match_name);
		}
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
		matched = idx->op.do_match(config, h80211, h80211len, ri);
		// return matched config.
		if (matched) {
			return matched;
		}
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
		MatchModules->match_name = strdup(match_name);
		MatchModules->op.init_match = op->init_match;
		MatchModules->op.do_match = op->do_match;
		MatchModules->op.finish_match = op->finish_match;
	}
	else {
		while (idx->next) {
			if (!strcasecmp(idx->match_name, match_name)) {
				echo.f("Duplicate match module. %s", match_name);
			}
			idx = idx->next;
		}

		idx->next = alloc_sizeof(Match_module_t);
		idx->enable = false;
		idx->match_name = strdup(match_name);
		idx->op.init_match = op->init_match;
		idx->op.do_match = op->do_match;
		idx->op.finish_match = op->finish_match;
	}
	echo.d("register match module [%s]", match_name);
}



extern void setup_h80211_match_module();

void setup_match_modules()
{
	setup_h80211_match_module();
}
