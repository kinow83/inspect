#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "format.h"
#include "parser.h"
#include "log.h"
#include "ezxml/ezxml.h"


static action_t *xml_action_parser(ezxml_t xml) {
	action_t *action;
	ezxml_t e;

	action = (action_t *)calloc(1, sizeof(action_t));

	// enable
	e = ezxml_child(xml, "enable");
	if (e && e->txt) {
		action->enable = atoi(e->txt);
	}
	// desc
	e = ezxml_child(xml, "name");
	if (e && e->txt) {
		action->name = strdup(e->txt);
	}
	// dwell
	e = ezxml_child(xml, "dwell");
	if (e && e->txt) {
		action->dwell = atoi(e->txt);
	}

	// channel
	e = ezxml_child(xml, "channel");
	if (e && e->txt) {
		action->channel = (uint8_t)atoi(e->txt);
	}

	// check mandatory config
	if (action->enable == 0 || action->name[0] == 0 
		|| action->channel == 0 || action->dwell == 0) {
		free_action(action);
		return NULL;
	}

	// IEEE 802.11 mac frame type
	e = ezxml_child(xml, "type");
	if (e && e->txt) {
		action->type = (uint8_t)atoi(e->txt);
		cv_enable(action->type);
	}
	// IEEE 802.11 mac frame subtype
	e = ezxml_child(xml, "subtype");
	if (e && e->txt) {
		action->subtype = (uint8_t)atoi(e->txt);
		cv_enable(action->subtype);
	}

	// from ds / to ds
	e = ezxml_child(xml, "fromds");
	if (e && e->txt) {
		action->fromds = (uint8_t)atoi(e->txt);
		cv_enable(action->fromds);


	return action;
}

static config_t *xml_config_parser(const char *filename, ezxml_t xml) {
	ezxml_t e;
	uint32_t version = 0;
	config_t *config;
	action_t *action;

	config = (config_t *)calloc(1, sizeof(*config));

	e = ezxml_child(xml, "version");
	if (e && e->txt) {
		version = atoi(e->txt);
	}

	if (version > CUR_VERSION) {
		free_config(config);
		echo.f("version mismatch current %d in %s", 
		CUR_VERSION, filename);
	}

	// multi action in config.
	for (e = ezxml_child(xml, "action"); e; e = e->next) {
		action = xml_action_parser(xml);
		if (action) {
			if (config->action) {
				action->next = config->action->next;
				config->action = action;
			} 
			else {
				config->action = action;
			}
		}
	}

	if (!config->action) {
		free_config(config);
		echo.f("empty config file %s", filename);
	}
	return config;
}

/*
 * name: xml_config_load
 * desc: XML 설정 파일을 불러온다.
 */
static config_t *xml_config_load(const char *filename) {
	config_t *config;
	ezxml_t xml;
	
	xml  = ezxml_parse_file(filename);
	if (!xml) {
		echo.f("%s is not config file", filename);
	}

	config = xml_config_parser(filename, xml);

	ezxml_free(xml);
	return config;
}

static void xml_action_debug(action_t *action) {
	echo.i("no = %d", action->no);
	echo.i("name = %s", action->name);
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

static void xml_config_debug(config_t *config) {
	action_t *action;

	if (!config) {
		return;
	}

	echo.i("version = %u\n", config->version);
	action = config->action;
	while (action) {
		xml_action_debug(action);
		action = action->next;
	}
}

void init_xml_config() {
	config_operation_t xml_config = {
		.load = xml_config_load,
		.debug = xml_config_debug,
	};
	
	register_config("xml", &xml_config);
}

