#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "format.h"
#include "parser.h"
#include "log.h"
#include "alloc.h"
#include "h80211_match.h"


static int h80211_test(Action_t *action, uint8_t *h80211, size_t h80211len, struct rx_info *ri)
{

	return 0;
}


static Config_t *h80211_match(Config_t *config, uint8_t *h80211, size_t h80211len, struct rx_info *ri)
{
	Config_t *cur, *match;

	cur = config;
	while (cur) {
		// test match all configs.
		if (cur->enable) {
			match = h80211_test(config, h80211, h80211len, ri);
			if (match) {
				return match;
			}
		}
		cur = cur->next;
	}

	return NULL;
}

void init_h80211_match_module()
{
	register_match_module("h80211", h80211_match);
}
