#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "format.h"
#include "parser.h"
#include "log.h"
#include "alloc.h"
#include "ezxml/ezxml.h"
#include "strings.h"
#include "h80211_struct.h"
#include "resource.h"

/*

 */
static char *xmlfilename;


static bool verify_capture_action_detail(Action_details_t *action)
{

	return true;
}

static bool verify_shooter_action_detail(Action_details_t *detail)
{
	if (cv_enabled(detail->protect)) {
		if (!cv_enabled(detail->fromds) && !cv_enabled(detail->tods)) {
			echo.e("xml_parser: set proctect, but not set fromds or tods");
			return false;
		}
	}
	if (!cv_enabled(detail->type)) {
		echo.e("xml_parser: not set type");
		return false;
	}
	else {
		if (VERIFY_WLAN_FRAME(detail->type)) {
			echo.e("xml_parser: unknown type: %d", detail->type);
			return false;
		}
	}
	if (!cv_enabled(detail->subtype)) {
		echo.e("xml_parser: not set subtype");
		return false;
	}
	else {
		switch (detail->type) {
		case WLAN_FC_TYPE_MGMT:
			if (!VERIFY_WLAN_FRAME_MGNT(detail->subtype)) {
				echo.e("xml_parser: unknown mgnt subtype: %d", detail->subtype);
				return false;
			}
			break;
		case WLAN_FC_TYPE_CTRL:
			if (!VERIFY_WLAN_FRAME_CTRL(detail->subtype)) {
				echo.e("xml_parser: unknown ctrl subtype: %d", detail->subtype);
				return false;
			}
			break;
		case WLAN_FC_TYPE_DATA:
			if (!VERIFY_WLAN_FRAME_DATA(detail->subtype)) {
				echo.e("xml_parser: unknown data subtype: %d", detail->subtype);
				return false;
			}
			break;
		}
	}

	switch (detail->type) {
	case WLAN_FC_TYPE_MGMT:
		if (cv_enabled(detail->ap_addr) && !cv_enabled(detail->st_addr)) {
			echo.e("xml_parser: not set st_addr");
		}
		if (!cv_enabled(detail->ap_addr) && cv_enabled(detail->st_addr)) {
			echo.e("xml_parser: not set ap_addr");
		}
		break;
	case WLAN_FC_TYPE_CTRL:
		break;
	case WLAN_FC_TYPE_DATA:
		break;
	}

	switch (detail->type) {
	case WLAN_FC_TYPE_MGMT:
		break;
	case WLAN_FC_TYPE_CTRL:
		break;
	case WLAN_FC_TYPE_DATA:
		break;
	}

	return true;
}

static bool verify_action(Config_t *config)
{
	Action_t *action;

	action = config->action;

	while (action) {
		if (verify_action(action) == false) {
			return false;
		}
		action = action->next;
	}
	return true;
}

static Action_details_t *xml_action_detail_parsing(const char *filename, ezxml_t xml)
{
	ezxml_t e;
	Action_details_t *detail;

	detail = alloc_sizeof(Action_details_t);

	// IEEE 802.11 mac frame type
	e = ezxml_child(xml, "type");
	if (e && e->txt) {
		detail->type = (u8)atoi(e->txt);
		cv_enable(detail->type);
	}
	// IEEE 802.11 mac frame subtype
	e = ezxml_child(xml, "subtype");
	if (e && e->txt) {
		detail->subtype = (u8)atoi(e->txt);
		cv_enable(detail->subtype);
	}
	// from ds / to ds
	e = ezxml_child(xml, "fromds");
	if (e && e->txt) {
		detail->fromds = (u8)atoi(e->txt);
		cv_enable(detail->fromds);
	}
	e = ezxml_child(xml, "tods");
	if (e && e->txt) {
		detail->tods = (u8)atoi(e->txt);
		cv_enable(detail->tods);
	}


	return detail;
}

static int action_id = 0;
static Action_t *xml_action_parsing(const char *filename, ezxml_t xml)
{
	Action_details_t *capture = NULL;
	Action_details_t *tmp, *last;
	Action_t *action;
	ezxml_t e;

	action = alloc_sizeof(Action_t);

	// enable
	e = ezxml_child(xml, "enable");
	if (e && e->txt) {
		action->enable = atoi(e->txt);
	} else {
		echo.f("empty 'enable' of action in %s", filename);
	}
	if (!action->enable) {
		return NULL;
	}
	// action id
	e = ezxml_child(xml, "aid");
	if (e && e->txt) {
		action->id = atoi(e->txt);
	} else {
		action->id = (++action_id);
	}
	// action name
	e = ezxml_child(xml, "name");
	if (e && e->txt) {
		action->name = strdup(e->txt);
	}
	// interval and check mandatory action.
	e = ezxml_child(xml, "interval");
	if (e && e->txt) {
		action->interval = atoi(e->txt);
	} else {
		echo.f("empty 'interval' of action in %s", filename);
	}
	// channel and check mandatory action.
	e = ezxml_child(xml, "channel");
	if (e && e->txt) {
		action->channel = (u8)atoi(e->txt);
	} else {
		echo.f("empty 'channel' of action in %s", filename);
	}

	// shooter
	e = ezxml_child(xml, "shooter");
	if (e) {
		action->shooter = xml_action_detail_parsing(filename, e);
	}

	// capture
	for (e = ezxml_child(xml, "capture"); e; e = e->next) {
		tmp = xml_action_detail_parsing(filename, e);

		if (!action->capture) {
			action->capture = tmp;
		}
		else {
			last = action->capture;
			while (last->next) {
				last = last->next;
			}
			last->next = tmp;
		}
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
static Config_t *do_xml_parser(void)
{
	Config_t *config;
	ezxml_t xml;
	
	xml  = ezxml_parse_file(xmlfilename);
	if (!xml) {
		echo.f("%s is not config file", xmlfilename);
	}

	config = xml_config_parsing(xmlfilename, xml);

	ezxml_free(xml);
	return config;
}

static void init_xml_parser(char *options)
{
	echo.d("init_xml_parser: %s", options);

	char **chunk, **field;
	int nchunk, nfield;
	int i;

	chunk = new_splits(options, ",", &nchunk);
	if (!chunk || nchunk == 0) {
		echo.f("error xml_parser: invalid option: %s", options);
	}
	for (i=0; i<nchunk; i++) {
		field = new_splits(chunk[i], "=", &nfield);
		if (!field || nfield != 2) {
			echo.f("error xml_parser: invalid option: %s", chunk[i]);
		}
		// xml filename
		if (!strcasecmp("filename", field[0])) {
			xmlfilename = strdup(field[1]);
		}
		free_splits(field, nfield);
	}
	free_splits(chunk, nchunk);

	if (!xmlfilename || strlen(xmlfilename) == 0) {
		echo.f("error xml_parser: empty filename: %s", options);
	}

	echo.i("[xml parser options]");
	echo.i("filename = %s", xmlfilename);
}

static void finish_xml_parser(void)
{
	echo.d("finish_xml_parser");

	if (xmlfilename) {
		free(xmlfilename);
	}
}

static const char *usage_xml_parser(void)
{
	return "xml:filename=<config file>";
}

void setup_xml_parser_module(void)
{
	Parser_operations_t op = {
		.init_parser   = init_xml_parser,
		.do_parser     = do_xml_parser,
		.finish_parser = finish_xml_parser,
		.usage_parser  = usage_xml_parser,
	};

	register_parser_module("xml", &op);
}

