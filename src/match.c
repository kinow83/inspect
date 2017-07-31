#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "format.h"
#include "log.h"
#include "alloc.h"
#include "match.h"

Match_module_t *MatchModules;


void init_match(void *resource)
{
	Match_module_t *idx;

	idx = MatchModules;
	while (idx) {
		idx->op.init_match(resource);
	}
}

void done_match(void *resource)
{
	Match_module_t *idx;

	idx = MatchModules;
	while (idx) {
		idx->op.done_match(resource);
	}
}

/*
 * Fast matching for all match modules.
 */
Config_t *do_match(Config_t *config, u8 *h80211, size_t h80211len, struct rx_info *ri)
{
	Match_module_t *idx;
	Config_t *match;

	idx = MatchModules;
	while (idx) {
		match = idx->op.do_match(config, h80211, h80211len, ri);
		// return matched config.
		if (match) {
			return match;
		}
	}
	return NULL;
}

void register_match_module(const char *match_name, Match_operations_t *op)
{
	Match_module_t *idx;

	idx = MatchModules;
	if (!idx) {
		MatchModules = alloc1(Match_module_t);
		MatchModules->match_name = strdup(match_name);
		MatchModules->op.init_match = op->init_match;
		MatchModules->op.do_match = op->do_match;
		MatchModules->op.done_match = op->done_match;
	}
	else {
		while (idx->next) {
			if (!strcasecmp(idx->match_name, match_name)) {
				echo.f("Duplicate match module. %s", match_name);
			}
			idx = idx->next;
		}

		idx->next = alloc1(Match_module_t);
		idx->match_name = strdup(match_name);
		idx->op.init_match = op->init_match;
		idx->op.do_match = op->do_match;
		idx->op.done_match = op->done_match;
	}
}



extern void init_h80211_match_module();

void init_match_modules()
{
	init_h80211_match_module();
}
