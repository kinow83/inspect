#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "format.h"
#include "match.h"
#include "log.h"
#include "alloc.h"


static int match_test(Action_t *action, uint8_t *h80211, size_t h80211len, struct rx_info *ri)
{

	return 0;
}


static Action_t *do_wifi_match(Config_t *config, uint8_t *h80211, size_t h80211len, struct rx_info *ri)
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

static void init_wifi_match(char *options)
{
	echo.d("init_wifi_match: %s", options);
}

static void finish_wifi_match()
{
	echo.d("finish_wifi_match");
}

static const char *usage_wifi_match(void)
{
	return "";
}

void setup_wifi_match_module()
{
	Match_operations_t op = {
			.init_match   = init_wifi_match,
			.do_match     = do_wifi_match,
			.finish_match = finish_wifi_match,
			.usage_match  = usage_wifi_match,
	};

	register_match_module("wifi", &op);
}
