#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "format.h"
#include "parser.h"
#include "log.h"
#include "alloc.h"
#include "ezxml/ezxml.h"


static Action_t *xml_action_parsing(const char *filename, ezxml_t xml)
{
	Action_t *action;
	ezxml_t e;

	action = alloc_sizeof(Action_t);

	// enable and check mandatory action.
	e = ezxml_child(xml, "enable");
	if (e && e->txt) {
		action->enable = atoi(e->txt);
	} else {
		echo.f("empty 'enable' of action in %s", filename);
	}
	// action name and check mandatory action.
	e = ezxml_child(xml, "name");
	if (e && e->txt) {
		action->name = strdup(e->txt);
	} else {
		echo.f("empty 'name' of action in %s", filename);
	}
	// dwell and check mandatory action.
	e = ezxml_child(xml, "dwell");
	if (e && e->txt) {
		action->dwell = atoi(e->txt);
	} else {
		echo.f("empty 'dwell' of action in %s", filename);
	}
	// channel and check mandatory action.
	e = ezxml_child(xml, "channel");
	if (e && e->txt) {
		action->channel = (u8)atoi(e->txt);
	} else {
		echo.f("empty 'channel' of action in %s", filename);
	}

	e = ezxml_child(xml, "no");
	if (e && e->txt) {
		action->no = (u8)atoi(e->txt);
	}
	// IEEE 802.11 mac frame type
	e = ezxml_child(xml, "type");
	if (e && e->txt) {
		action->type = (u8)atoi(e->txt);
		cv_enable(action->type);
	}
	// IEEE 802.11 mac frame subtype
	e = ezxml_child(xml, "subtype");
	if (e && e->txt) {
		action->subtype = (u8)atoi(e->txt);
		cv_enable(action->subtype);
	}

	// from ds / to ds
	e = ezxml_child(xml, "fromds");
	if (e && e->txt) {
		action->fromds = (u8)atoi(e->txt);
		cv_enable(action->fromds);
	}

	return action;
}

static Config_t *xml_config_parsing(const char *filename, ezxml_t xml)
{
	ezxml_t e;
	u32 version = 0;
	Config_t *config;
	Action_t *action;

	config = alloc_sizeof(Config_t);

	e = ezxml_child(xml, "version");
	if (e && e->txt) {
		version = (u32)atoi(e->txt);
	}
	if (version > CUR_VERSION) {
		free_config(config);
		echo.f("version mismatch current %d in %s", CUR_VERSION, filename);
	}
	config->version = version;

	e = ezxml_child(xml, "name");
	if (!e || !e->txt) {
		free_config(config);
		echo.f("empty 'name'of config in %s", CUR_VERSION, filename);
	}
	config->name = strdup(e->txt);

	// multi action in config.
	for (e = ezxml_child(xml, "action"); e; e = e->next) {
		action = xml_action_parsing(filename, e);
		if (action) {
			if (config->action) {
				action->next = config->action;
				config->action = action;
			} 
			else {
				config->action = action;
			}
		}
	}

	if (!config->action) {
		free_config(config);
		echo.f("empty 'action' of config in %s", filename);
	}
	return config;
}

/*
 * name: xml_config_load
 * desc: XML 설정 파일을 불러온다.
 */
static Config_t *do_xml_parser(char *args)
{
	Config_t *config;
	ezxml_t xml;
	char *filename = args;
	
	xml  = ezxml_parse_file(filename);
	if (!xml) {
		echo.f("%s is not config file", filename);
	}

	config = xml_config_parsing(filename, xml);

	ezxml_free(xml);
	return config;
}

static void init_xml_parser(char *options)
{
	echo.d("init_xml_parser: %s", options);
}

static void done_xml_parser(void)
{
	echo.d("done_xml_parser");
}

void setup_xml_parser_module(void)
{
	Parser_operations_t op = {
			.do_parser   = do_xml_parser,
			.init_parser = init_xml_parser,
			.done_parser = done_xml_parser,
	};

	register_parser_module("xml", &op);
}

