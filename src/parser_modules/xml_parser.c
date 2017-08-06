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

static Action_details_t *xml_capture_detail_parsing(
		const char *filename, Action_t *action ,ezxml_t xml)
{

}


static Tag_t *xml_tag_parsing(
		const char *filename, Action_details_t *detail, ezxml_t xml)
{

}

static Action_details_t *xml_shooter_detail_parsing(
		const char *filename, Action_t *action ,ezxml_t xml)
{
	ezxml_t e;
	Action_details_t *detail;

	detail = alloc_sizeof(Action_details_t);

	detail->action = action;

	// name of config
	e = ezxml_child(xml, "name");
	if (e && e->txt) {
		detail->name = strdup(e->txt);
	} else {
		detail->name = alloc_type(char, 1);
	}

	// id of config
	e = ezxml_child(xml, "id");
	if (e && e->txt) {
		detail->id = (u8)atoi(e->txt);
	} else {
		echo.f("xml_parer: empty 'id' [detail:%s, action:%s, file:%s]",
				detail->name, action->name, filename);
	}

	// IEEE 802.11 mac frame type
	e = ezxml_child(xml, "type");
	if (e && e->txt) {
		detail->type = (u8)atoi(e->txt);
		cv_enable(detail->type);
	} else {
		echo.f("xml_parer: empty 'type' [detail:%s, action:%s, file:%s]",
				detail->name, action->name, filename);
	}
	if(!VERIFY_WLAN_FRAME(detail->type)) {
		echo.f("xml_parer: invalid 'type=%d' [detail:%s, action:%s, file:%s]",
				detail->type, detail->name, action->name, filename);
	}

	// IEEE 802.11 mac frame subtype
	e = ezxml_child(xml, "subtype");
	if (e && e->txt) {
		detail->subtype = (u8)atoi(e->txt);
		cv_enable(detail->subtype);
	} else {
		echo.f("xml_parer: empty 'subtype' [detail:%s, action:%s, file:%s]",
				detail->name, action->name, filename);
	}
	switch (detail->type) {
	case WLAN_FC_TYPE_MGMT:
		if (!VERIFY_WLAN_FRAME_MGNT(detail->subtype)) {
			echo.f("xml_parer: invalid 'subtype=%d' [detail:%s, action:%s, file:%s]",
					detail->subtype, detail->name, action->name, filename);
		}
		break;
	case WLAN_FC_TYPE_CTRL:
		if (!VERIFY_WLAN_FRAME_CTRL(detail->subtype)) {
			echo.f("xml_parer: invalid 'subtype=%d' [detail:%s, action:%s, file:%s]",
					detail->subtype, detail->name, action->name, filename);
		}
		break;
	case WLAN_FC_TYPE_DATA:
		if (!VERIFY_WLAN_FRAME_DATA(detail->subtype)) {
			echo.f("xml_parer: invalid 'subtype=%d' [detail:%s, action:%s, file:%s]",
					detail->subtype, detail->name, action->name, filename);
		}
		break;
	}

	// from ds / to ds
	e = ezxml_child(xml, "fromds");
	if (e && e->txt) {
		detail->fromds = (u8)atoi(e->txt);
		cv_enable(detail->fromds);
	} else {
		if (detail->type == WLAN_FC_TYPE_DATA) {
			echo.f("xml_parer: empty 'fromds' [detail:%s, action:%s, file:%s]",
				detail->name, action->name, filename);
		}
	}
	e = ezxml_child(xml, "tods");
	if (e && e->txt) {
		detail->tods = (u8)atoi(e->txt);
		cv_enable(detail->tods);
	} else {
		if (detail->type == WLAN_FC_TYPE_DATA) {
			echo.f("xml_parer: empty 'tods' [detail:%s, action:%s, file:%s]",
				detail->name, action->name, filename);
		}
	}
	// targter or source mac address
	{
		e = ezxml_child(xml, "ap_addr");
		if (e && e->txt) {
			if (!str2mac(e->txt, detail->ap_addr)) {
				echo.f("xml_parer: invalid 'ap_addr' [detail:%s, action:%s, file:%s]",
						detail->name, action->name, filename);
			}
			cv_enable(detail->ap_addr);
		}
		e = ezxml_child(xml, "st_addr");
		if (e && e->txt) {
			if (!str2mac(e->txt, detail->st_addr)) {
				echo.f("xml_parer: invalid 'st_addr' [detail:%s, action:%s, file:%s]",
						detail->name, action->name, filename);
			}
			cv_enable(detail->st_addr);
		}
		e = ezxml_child(xml, "any_addr");
		if (e && e->txt) {
			if (!str2mac(e->txt, detail->any_addr)) {
				echo.f("xml_parer: invalid 'any_addr' [detail:%s, action:%s, file:%s]",
						detail->name, action->name, filename);
			}
			cv_enable(detail->any_addr);
		}
		e = ezxml_child(xml, "addr1");
		if (e && e->txt) {
			if (!str2mac(e->txt, detail->addr1)) {
				echo.f("xml_parer: invalid 'addr1' [detail:%s, action:%s, file:%s]",
						detail->name, action->name, filename);
			}
			cv_enable(detail->addr1);
		}
		e = ezxml_child(xml, "addr2");
		if (e && e->txt) {
			if (!str2mac(e->txt, detail->addr2)) {
				echo.f("xml_parer: invalid 'addr2' [detail:%s, action:%s, file:%s]",
						detail->name, action->name, filename);
			}
			cv_enable(detail->addr2);
		}
		e = ezxml_child(xml, "addr3");
		if (e && e->txt) {
			if (!str2mac(e->txt, detail->addr3)) {
				echo.f("xml_parer: invalid 'addr3' [detail:%s, action:%s, file:%s]",
						detail->name, action->name, filename);
			}
			cv_enable(detail->addr3);
		}
		e = ezxml_child(xml, "addr4");
		if (e && e->txt) {
			if (!str2mac(e->txt, detail->addr4)) {
				echo.f("xml_parer: invalid 'addr4' [detail:%s, action:%s, file:%s]",
						detail->name, action->name, filename);
			}
			cv_enable(detail->addr4);
		}

		switch (detail->type) {
		case WLAN_FC_TYPE_MGMT:
			if (cv_enabled(detail->ap_addr) ^ cv_enabled(detail->st_addr)) {
				echo.f("xml_parer: mgnt frame. but empty 'ap_addr' or 'st_addr' "
						"[detail:%s, action:%s, file:%s]",
						detail->name, action->name, filename);
			} else {
				if (!cv_enabled(detail->addr1) ||
						!cv_enabled(detail->addr2) ||
						!cv_enabled(detail->addr3)) {
					echo.f("xml_parer: mgnt frame. but empty 'addr1' ~ 'addr3' "
							"[detail:%s, action:%s, file:%s]",
							detail->name, action->name, filename);
				}
			}
			break;
		case WLAN_FC_TYPE_CTRL:
			switch (detail->subtype) {
			case WLAN_FC_STYPE_BLOCK_ACK_REQ:
			case WLAN_FC_STYPE_BLOCK_ACK:
				if (!cv_enabled(detail->addr1) ||
						!cv_enabled(detail->addr2) ||
						!cv_enabled(detail->addr3)) {
					echo.f("xml_parer: ctrl block ack frame. but empty 'addr1' ~ 'addr3' "
							"[detail:%s, action:%s, file:%s]",
							detail->name, action->name, filename);
				}
				break;
			case WLAN_FC_STYPE_PSPOLL:
			case WLAN_FC_STYPE_RTS:
			case WLAN_FC_STYPE_CTS:
			case WLAN_FC_STYPE_ACK:
			case WLAN_FC_STYPE_CFEND:
			case WLAN_FC_STYPE_CFENDACK:

				break;
			}
			break;
		case WLAN_FC_TYPE_DATA:
			if (cv_enabled(detail->ap_addr) ^ cv_enabled(detail->st_addr)) {
				echo.f("xml_parer: data frame. but empty 'ap_addr' or 'st_addr' "
						"[detail:%s, action:%s, file:%s]",
						detail->name, action->name, filename);
			} else {
				if (!cv_enabled(detail->addr1) ||
						!cv_enabled(detail->addr2) ||
						!cv_enabled(detail->addr3)) {
					echo.f("xml_parer: data frame. but empty 'addr1' ~ 'addr3' "
							"[detail:%s, action:%s, file:%s]",
							detail->name, action->name, filename);
				}
			}
			// check WDS address
			if (detail.fromds == 1 && detail->tods == 1) {
				if (!cv_enabled(detail->addr1) ||
						!cv_enabled(detail->addr2) ||
						!cv_enabled(detail->addr3) ||
						!cv_enabled(detail->addr4)) {
					echo.f("xml_parer: invalid WDS address (addr1~addr4) "
							"[detail:%s, action:%s, file:%s]",
							detail->name, action->name, filename);
				}
			}
			if (detail.fromds == 0 && detail->tods == 0) {
				if (!cv_enabled(detail->addr1) ||
						!cv_enabled(detail->addr2) ||
						!cv_enabled(detail->addr3)) {
					echo.f("xml_parer: invalid ADHOC address (addr1~addr3) "
							"[detail:%s, action:%s, file:%s]",
							detail->name, action->name, filename);
				}
			}
			break;
		}
	}
	// protect for fata frame
	e = ezxml_child(xml, "protect");
	if (e && e->txt) {
		detail->protect = atoi(e->txt) == 0 ? 0 : 1;
		if (detail->protect) {
			if (detail->type != WLAN_FC_TYPE_DATA) {
				echo.f("xml_parer: invalid 'protect'. but not data frame "
						"[detail:%s, action:%s, file:%s]",
						detail->name, action->name, filename);
			}
			cv_enable(detail->protect);
		}
	}
	// duration
	e = ezxml_child(xml, "duration");
	if (e && e->txt) {
		detail->duration = atoi(e->txt);
		cv_enable(detail->duration);
	}
	// 802.11 MAC FRAME version
	e = ezxml_child(xml, "version");
	if (e && e->txt) {
		detail->version = atoi(e->txt);
		cv_enable(detail->version);
	}
	// ibss
	e = ezxml_child(xml, "ibss");
	if (e && e->txt) {
		detail->ibss = atoi(e->txt) == 0 ? 0 : 1;
		if (detail->ibss) {
			cv_enable(detail->ibss);
		}
	}



	return detail;
}

static Action_t *xml_action_parsing(const char *filename, ezxml_t xml)
{
	Action_details_t *capture = NULL;
	Action_details_t *tmp, *last;
	Action_t *action;
	ezxml_t e;
	const char *NIL = "<empty>";

	action = alloc_sizeof(Action_t);

	// enable
	e = ezxml_child(xml, "enable");
	if (e && e->txt) {
		action->enable = atoi(e->txt);
	} else {
		action->enable = 1;
	}
	if (!action->enable) {
		return NULL;
	}
	// action id
	e = ezxml_child(xml, "id");
	if (e && e->txt) {
		action->id = atoi(e->txt);
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
	}
	// channel and check mandatory action.
	e = ezxml_child(xml, "channel");
	if (e && e->txt) {
		action->channel = (u8)atoi(e->txt);
	}

	// shooter
	e = ezxml_child(xml, "shooter");
	if (e) {
		action->shooter = xml_shooter_detail_parsing(filename, action ,e);
	}

	// capture
	for (e = ezxml_child(xml, "capture"); e; e = e->next) {
		tmp = xml_capture_detail_parsing(filename, action, e);

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
		echo.f("empty config 'name' [file :%s]", CUR_VERSION, filename);
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

	if (get_action_count(config) == 0) {
		free_config(config);
		echo.f("empty config 'action' [file: %s]", filename);
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

