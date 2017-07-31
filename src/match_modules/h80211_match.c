#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "format.h"
#include "parser.h"
#include "log.h"
#include "alloc.h"


static int match_test(Action_t *action, uint8_t *h80211, size_t h80211len, struct rx_info *ri)
{

	return 0;
}


static Action_t *do_h80211_match(Config_t *config, uint8_t *h80211, size_t h80211len, struct rx_info *ri)
{
	Action_t *action, *matched;

	action = config->action;
	while (action) {
		// test match all configs.
		if (action->enable) {
			if (match_test(action, h80211, h80211len, ri)) {
				return action;
			}
		}
		action = action->next;
	}

	return NULL;
}

static void init_h80211_match(void *resource)
{
	echo.d("init_h80211_match");
}

static void done_h80211_match(void *resource)
{
	echo.d("done_h80211_match");
}

void init_h80211_match_module()
{
	Match_operations_t op = {
			.init_match = init_h80211_match,
			.do_match = do_h80211_match,
			.done_match = done_h80211_match,
	};

	register_match_module("h80211", &op);
}
