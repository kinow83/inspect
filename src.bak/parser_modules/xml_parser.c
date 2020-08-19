#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "format.h"
#include "parser.h"
#include "log.h"
#include "alloc.h"
#include "ezxml/ezxml.h"
#include "string_util.h"
#include "h80211_struct.h"
#include "resource.h"
#include "convert.h"

/*

 */
static char *xmlfilename;

/*
 * resource.c
 */
extern const char* h80211_ctrl_string[];
extern const char* h80211_mgnt_string[];
extern const char* h80211_frame_string[];

static Tag_t *xml_tag_parsing(Action_details_t *detail, ezxml_t xml)
{
	Tag_t *tag, *first, *tmp;
	ezxml_t e;
	bool hexmode;
	const char *id, *len, *data, *type;

	first = NULL;

	for (e = ezxml_child(xml, "tag"); e; e = e->next) {
		id   = ezxml_attr(e, "id");
		len  = ezxml_attr(e, "len");
		data = ezxml_attr(e, "data");
		type = ezxml_attr(e, "type");

		/*
		 * tag two mode is 'hex' and 'string'
		 * Default 'string' mode
		 */
		hexmode = 0;
		if (type && (!strcasecmp(type, "hex"))) {
			hexmode = 1;
		}

		if (!id) {
			echo.f("xml_parer: missing 'id' of tag "
					"[detail:%s, action:%s, file:%s]",
					detail->name, detail->action->name,
					detail->action->config->filename);
		}
		if (!len) {
			echo.f("xml_parer: missing 'len' of tag "
					"[id: %s, detail:%s, action:%s, file:%s]",
					id, detail->name, detail->action->name,
					detail->action->config->filename);
		}
		if (!data) {
			echo.f("xml_parer: missing 'data' of tag "
					"[id: %s, detail:%s, action:%s, file:%s]",
					id, detail->name, detail->action->name,
					detail->action->config->filename);
		}

		tag = alloc_sizeof(Tag_t);
		tag->id = atoi(id);
		tag->len = atoi(len);
		tag->data = alloc_type(u8, tag->len+1);
		tag->next = NULL;

		if (hexmode) {
			tag->type = TAG_TYPE_HEX;
			int n = hex2binarray(data, tag->data, tag->len);
		}
		else {
			tag->type = TAG_TYPE_STR;
			strncpy((char *)tag->data, data, tag->len);
			tag->data[tag->len] = '\0';
		}

		// tag linked list
		if (!first) {
			first = tag;
		}
		else {
			tmp = first;
			while (tmp->next) {
				tmp = tmp->next;
			}
			tmp->next = tag;
		}
	}
	return first;
}

/**
 * In case capture, not check bellow
 * - subtype
 * - from ds / to ds
 * - da / sa / addr1 / addr2 / addr3 / addr4 / any_addr
 */
static Action_details_t *xml_detail_parsing(Action_t *action ,ezxml_t xml, bool is_shooter)
{
	ezxml_t e;
	Action_details_t *detail;
	const char *typename = NULL;
	const char *d_name, *a_name, *c_name, *filename;

	a_name = action->name;
	c_name = action->config->name;
	filename = action->config->filename;

	detail = alloc_sizeof(Action_details_t);

	detail->action = action;

	// name of config
	e = ezxml_child(xml, "name");
	if (e && e->txt) {
		detail->name = strdup(e->txt);
	}
	else {
		detail->name = alloc_type(char, 1);
	}
	d_name = detail->name;

	// id of config
	e = ezxml_child(xml, "id");
	if (e && e->txt) {
		detail->id = (u8)atoi(e->txt);
	}
	else {
		echo.f("xml_parer: missing 'id' "
				"[detail:%s, action:%s, config:%s, file:%s]",
				d_name, a_name, c_name, filename);
	}

	/*
	 * can build malformed packet
	 */
	e = ezxml_child(xml, "malformable");
	if (e && e->txt) {
		detail->malformable = (u8)atoi(e->txt);
	}
	else {
		detail->malformable = 0;
	}
	// 802.11 MAC FRAME version
	e = ezxml_child(xml, "version");
	if (e && e->txt) {
		detail->version = atoi(e->txt);
		cv_enable(detail->version);
	}
	// duration
	e = ezxml_child(xml, "duration");
	if (e && e->txt) {
		detail->duration = atoi(e->txt);
		cv_enable(detail->duration);
	}
	// IEEE 802.11 frame type
	e = ezxml_child(xml, "type");
	if (e && e->txt) {
		if (is_number(e->txt)) {
			detail->type = (u8)atoi(e->txt);
			typename = get_80211_type_name(detail->type);
		}
		else {
			if (get_80211_type_code(e->txt, &detail->type) == false) {
				echo.f("xml_parer: unknown 'type' = %s "
						"[detail:%s, action:%s, config:%s, file:%s]",
						e->txt,
						d_name, a_name, c_name, filename);
			}
			typename = e->txt;
		}
		if(!VERIFY_WLAN_FRAME(detail->type)) {
			echo.f("xml_parer: invalid 'type=%d' "
					"[detail:%s, action:%s, config:%s, file:%s]",
					detail->type,
					d_name, a_name, c_name, filename);
		}
		cv_enable(detail->type);
	}
	else {
		/*
		 * shooter MUST set type and subtype.
		 */
		if (is_shooter) {
			echo.f("xml_parer: missing 'type' "
					"[detail:%s, action:%s, config:%s, file:%s]",
					d_name, a_name, c_name, filename);
		}
	}

	// IEEE 802.11 frame subtype
	e = ezxml_child(xml, "subtype");
	if (e && e->txt) {
		if (is_number(e->txt)) {
			// subtype 코드 번호로 지정
			detail->subtype = (u8)atoi(e->txt);
		}
		else {
			// subtype 이름으로 지정
			if (get_80211_subtype_code(typename, e->txt, NULL, &detail->subtype) == false) {
				echo.f("xml_parer: unknown 'subtype' = %s "
						"[detail:%s, action:%s, config:%s, file:%s]",
						e->txt,
						d_name, a_name, c_name, filename);
			}
		}

		if (!detail->malformable) {
			/****************************************************
			 *
			 * check validation 802.11 type and subtype
			 *
			 ****************************************************/
			switch (detail->type) {
			case WLAN_FC_TYPE_MGMT:
				if (!VERIFY_WLAN_FRAME_MGNT(detail->subtype)) {
					echo.f("xml_parer: invalid 'subtype=%d' "
							"[detail:%s, action:%s, config:%s, file:%s]",
							detail->subtype,
							d_name, a_name, c_name, filename);
				}
				break;
			case WLAN_FC_TYPE_CTRL:
				if (!VERIFY_WLAN_FRAME_CTRL(detail->subtype)) {
					echo.f("xml_parer: invalid 'subtype=%d' "
							"[detail:%s, action:%s, config:%s, file:%s]",
							detail->subtype,
							d_name, a_name, c_name, filename);
				}
				break;
			case WLAN_FC_TYPE_DATA:
				if (!VERIFY_WLAN_FRAME_DATA(detail->subtype)) {
					echo.f("xml_parer: invalid 'subtype=%d' "
							"[detail:%s, action:%s, config:%s, file:%s]",
							detail->subtype,
							d_name, a_name, c_name, filename);
				}
				break;
			}
		}

		cv_enable(detail->subtype);
	}
	else {
		/*
		 * shooter MUST set type and subtype.
		 */
		if (is_shooter) {
			echo.f("xml_parer: missing 'subtype' "
					"[detail:%s, action:%s, config:%s, file:%s]",
					d_name, a_name, c_name, filename);
		}
	}

	// from ds / to ds
	e = ezxml_child(xml, "fromds");
	if (e && e->txt) {
		detail->fromds = (u8)atoi(e->txt);
		cv_enable(detail->fromds);
	}
	else {
		/********************************************************************************
		 * DATA FRAME 경우 반드시 패킷의 방향 (fromds/tods) 있어야 한다.
		 * 단, adhoc인 경우는 bss내의 통신이므로 fromds=0, tods=0으로 설정해야 한다.
		 ********************************************************************************/
#if 0
		if (detail->type == WLAN_FC_TYPE_DATA) {
			if (is_shooter) {
				echo.f("xml_parer: missing 'fromds' "
					"[detail:%s, action:%s, config:%s, file:%s]",
					d_name, a_name, c_name, filename);
			}
		}
#endif
	}
	e = ezxml_child(xml, "tods");
	if (e && e->txt) {
		detail->tods = (u8)atoi(e->txt);
		cv_enable(detail->tods);
	}
	else {
#if 0
		if (detail->type == WLAN_FC_TYPE_DATA) {
			if (is_shooter) {
				echo.f("xml_parer: missing 'tods' "
						"[detail:%s, action:%s, config:%s, file:%s]",
						d_name, a_name, c_name, filename);
			}
		}
#endif
	}
	// target or source mac address
	e = ezxml_child(xml, "da");
	if (e && e->txt) {
		if (!str2mac(e->txt, detail->da)) {
			echo.f("xml_parer: invalid 'da' "
					"[detail:%s, action:%s, config:%s, file:%s]",
					d_name, a_name, c_name, filename);
		}
		cv_enable(detail->da);
	}
	e = ezxml_child(xml, "sa");
	if (e && e->txt) {
		if (!str2mac(e->txt, detail->sa)) {
			echo.f("xml_parer: invalid 'sa' "
					"[detail:%s, action:%s, config:%s, file:%s]",
					d_name, a_name, c_name, filename);
		}
		cv_enable(detail->sa);
	}
	e = ezxml_child(xml, "any_addr");
	if (e && e->txt) {
		if (!str2mac(e->txt, detail->any_addr)) {
			echo.f("xml_parer: invalid 'any_addr' "
					"[detail:%s, action:%s, config:%s, file:%s]",
					d_name, a_name, c_name, filename);
		}
		cv_enable(detail->any_addr);
	}
	e = ezxml_child(xml, "addr1");
	if (e && e->txt) {
		if (!str2mac(e->txt, detail->addr1)) {
			echo.f("xml_parer: invalid 'addr1' "
					"[detail:%s, action:%s, config:%s, file:%s]",
					d_name, a_name, c_name, filename);
		}
		cv_enable(detail->addr1);
	}
	e = ezxml_child(xml, "addr2");
	if (e && e->txt) {
		if (!str2mac(e->txt, detail->addr2)) {
			echo.f("xml_parer: invalid 'addr2' "
					"[detail:%s, action:%s, config:%s, file:%s]",
					d_name, a_name, c_name, filename);
		}
		cv_enable(detail->addr2);
	}
	e = ezxml_child(xml, "addr3");
	if (e && e->txt) {
		if (!str2mac(e->txt, detail->addr3)) {
			echo.f("xml_parer: invalid 'addr3' "
					"[detail:%s, action:%s, config:%s, file:%s]",
					d_name, a_name, c_name, filename);
		}
		cv_enable(detail->addr3);
	}
	e = ezxml_child(xml, "addr4");
	if (e && e->txt) {
		if (!str2mac(e->txt, detail->addr4)) {
			echo.f("xml_parer: invalid 'addr4' "
					"[detail:%s, action:%s, config:%s, file:%s]",
					d_name, a_name, c_name, filename);
		}
		cv_enable(detail->addr4);
	}
	/*
	 * check address
	 * (da, sa, any_addr, addr1, addr2, addr3, addr4)
	 */
	if (!detail->malformable) {
		if (is_shooter) {
			switch (detail->type) {
			case WLAN_FC_TYPE_MGMT:
				/*
				 * management frame 경우 SA 또는 DA가 설정되어 있는지 확인한다.
				 * SA 또는 DA가 설정되지 않은 경우에는
				 * ADDR1, ADDR2, ADDR3 이 설정되어야 한다.
				 */
				if (cv_enabled(detail->da) ^ cv_enabled(detail->sa)) {
					echo.f("xml_parer: mgnt frame. but missing 'da' or 'sa' "
							"[detail:%s, action:%s, config:%s, file:%s]",
							d_name, a_name, c_name, filename);
				}
				else if (!cv_enabled(detail->da) && !cv_enabled(detail->sa)) {
					if (!cv_enabled(detail->addr1) ||
							!cv_enabled(detail->addr2) ||
							!cv_enabled(detail->addr3)) {
						echo.f("xml_parer: mgnt frame. but missing 'addr1~addr3' "
								"[detail:%s, action:%s, config:%s, file:%s]",
								d_name, a_name, c_name, filename);
					}
				}
				break;
			case WLAN_FC_TYPE_CTRL:
				switch (detail->subtype) {
				/*
				 * pspoll, block_ack_req, block_ack, rts is 2 address
				 */
				case WLAN_FC_STYPE_PSPOLL:
				case WLAN_FC_STYPE_BLOCK_ACK_REQ:
				case WLAN_FC_STYPE_BLOCK_ACK:
				case WLAN_FC_STYPE_RTS:
					if (!cv_enabled(detail->addr1) ||
							!cv_enabled(detail->addr2)) {
						echo.f("xml_parer: %s(%d) frame. but missing 'addr1~addr2' "
								"[detail:%s, action:%s, config:%s, file:%s]",
								h80211_ctrl_string[detail->subtype], detail->subtype,
								d_name, a_name, c_name, filename);
					}
					break;
				/*
				 * cts, ack, cf_end, cd_end_ack is 1 address
				 */
				case WLAN_FC_STYPE_CTS:
				case WLAN_FC_STYPE_ACK:
				case WLAN_FC_STYPE_CFEND:
				case WLAN_FC_STYPE_CFENDACK:
					if (!cv_enabled(detail->da) && !cv_enabled(detail->addr1)) {
						echo.f("xml_parer: %s(%d) frame. but missing 'da' or 'addr1' "
								"[detail:%s, action:%s, config:%s, file:%s]",
								h80211_ctrl_string[detail->subtype], detail->subtype,
								d_name, a_name, c_name, filename);
					}
					break;
				}
				break;
			case WLAN_FC_TYPE_DATA:
				/*
				 * data frame 경우 SA 또는 DA가 설정되어 있는지 확인한다.
				 * SA 또는 DA가 설정되지 않은 경우에는
				 * ADDR1, ADDR2, ADDR3 이 설정되어야 한다.
				 */
				if (cv_enabled(detail->da) ^ cv_enabled(detail->sa)) {
					echo.f("xml_parer: data frame. but missing 'da' or 'sa' "
							"[detail:%s, action:%s, config:%s, file:%s]",
							d_name, a_name, c_name, filename);
				}
				else if (!cv_enabled(detail->da) && !cv_enabled(detail->sa)) {
					if (!cv_enabled(detail->addr1) ||
							!cv_enabled(detail->addr2) ||
							!cv_enabled(detail->addr3)) {
						echo.f("xml_parer: data frame. but missing 'addr1~addr3' "
								"[detail:%s, action:%s, config:%s, file:%s]",
								d_name, a_name, c_name, filename);
					}
				}
				// check WDS address and fromds/tods
				if (detail->fromds == 1 && detail->tods == 1) {
					if (!cv_enabled(detail->addr1) ||
							!cv_enabled(detail->addr2) ||
							!cv_enabled(detail->addr3) ||
							!cv_enabled(detail->addr4)) {
						echo.f("xml_parer: missing WDS address 'addr1~addr4' "
								"[detail:%s, action:%s, config:%s, file:%s]",
								d_name, a_name, c_name, filename);
					}
				}
				// check ADHOC address and fromds/tods
				if (detail->fromds == 0 && detail->tods == 0) {
					if (!cv_enabled(detail->addr1) ||
							!cv_enabled(detail->addr2) ||
							!cv_enabled(detail->addr3)) {
						echo.f("xml_parer: missing ADHOC address 'addr1~addr3' "
								"[detail:%s, action:%s, config:%s, file:%s]",
								d_name, a_name, c_name, filename);
					}
				}
				break;
			}
		}
	}
	// protect for data frame
	e = ezxml_child(xml, "protect");
	if (e && e->txt) {
		detail->protect = atoi(e->txt) == 0 ? 0 : 1;

		if (!detail->malformable) {
			/*
			 * protect field is only for data frame.
			 */
			if (detail->protect) {
				if (detail->type != WLAN_FC_TYPE_DATA) {
					echo.f("xml_parer: invalid 'protect'. but not data frame "
							"[detail:%s, action:%s, config:%s, file:%s]",
							d_name, a_name, c_name, filename);
				}
				cv_enable(detail->protect);
			}
		}
	}
	// ibss
	e = ezxml_child(xml, "ibss");
	if (e && e->txt) {
		detail->ibss = atoi(e->txt) == 0 ? 0 : 1;
		if (detail->ibss) {
			cv_enable(detail->ibss);
		}
	}
	if (!detail->malformable) {
		/*
		 * check ibss.
		 * only probe-resp, beacon, (re)assoc-req, (re)assoc-resp
		 */
		if (detail->ibss) {
			if (detail->type != WLAN_FC_TYPE_MGMT) {
				echo.f("xml_parer: ibss only management frame. "
						"[detail:%s, action:%s, config:%s, file:%s]",
						d_name, a_name, c_name, filename);
			}
			switch (detail->subtype) {
			case WLAN_FC_STYPE_PROBE_RESP:
			case WLAN_FC_STYPE_BEACON:
			case WLAN_FC_STYPE_ASSOC_REQ:
			case WLAN_FC_STYPE_ASSOC_RESP:
			case WLAN_FC_STYPE_REASSOC_REQ:
			case WLAN_FC_STYPE_REASSOC_RESP:
				break;
			default:
				echo.f("xml_parer: ibss not permit %s(%d) frame. "
						"(probe-resp,beacon,(re)assoc_req/resp)"
						"[detail:%s, action:%s, config:%s, file:%s]",
						h80211_mgnt_string[detail->subtype], detail->subtype,
						d_name, a_name, c_name, filename);
				break;
			}
		}
	}

	// deauth_reason
	e = ezxml_child(xml, "deauth_reason");
	if (e && e->txt) {
		detail->deauth_reason = atoi(e->txt);
		cv_enable(detail->ibss);
	}
	if (!detail->malformable) {
		/*
		 * check deauth reason code.
		 */
		if (cv_enabled(detail->deauth_reason)) {
			if (detail->type != WLAN_FC_TYPE_MGMT
					&& detail->subtype != WLAN_FC_STYPE_DEAUTH) {
				echo.f("xml_parer: can't set deauth reason in %s(%d). "
						"[detail:%s, action:%s, config:%s, file:%s]",
						h80211_mgnt_string[detail->subtype], detail->subtype,
						d_name, a_name, c_name, filename);
			}
		}
	}

	/*
	 * tag parsing
	 */
	e = ezxml_child(xml, "tags");
	if (e) {
		detail->tags = xml_tag_parsing(detail, e);
		if (detail->tags) {
			detail->tags = new_sort_tags(detail->tags);
		}
		else {
			echo.f("xml_parer: failed to sort tags "
					"[detail:%s, action:%s, config:%s, file:%s]",
					d_name, a_name, c_name, filename);
		}
	}

	return detail;
}

static Action_details_t *xml_capture_detail_parsing(Action_t *action ,ezxml_t xml)
{
	bool check_addr = false;
	return xml_detail_parsing(action, xml, check_addr);
}

static Action_details_t *xml_shooter_detail_parsing(Action_t *action ,ezxml_t xml)
{
	bool check_addr = true;
	return xml_detail_parsing(action, xml, check_addr);
}

static Action_t *xml_action_parsing(Config_t *config, ezxml_t xml)
{
	Action_details_t *capture = NULL;
	Action_details_t *tmp, *last;
	Action_t *action;
	ezxml_t e;
	const char *a_name, *c_name, *filename;

	c_name = config->name;
	filename = config->filename;

	action = alloc_sizeof(Action_t);

	action->config = config;

	// enable
	e = ezxml_child(xml, "enable");
	if (e && e->txt) {
		action->enable = atoi(e->txt);
	}
	else {
#if 0
		action->enable = 1;
#else
		echo.f("xml_parer: missing action 'enable' [config:%s, file:%s]",
				c_name, filename);
#endif
	}
	if (!action->enable) {
		return NULL;
	}
	// action name
	e = ezxml_child(xml, "name");
	if (e && e->txt) {
		action->name = strdup(e->txt);
	}
	else {
		echo.f("xml_parer: missing action 'name' [config:%s, file:%s]",
				c_name, filename);
	}
	a_name = action->name;

	// action id
	e = ezxml_child(xml, "id");
	if (e && e->txt) {
		action->id = atoi(e->txt);
	}
	else {
		echo.f("xml_parer: missing action 'id' [action:%s, config:%s, file:%s]",
				a_name, c_name, filename);
	}
	// interval
	e = ezxml_child(xml, "interval");
	if (e && e->txt) {
		action->interval = atoi(e->txt);
	}
	else {
		echo.f("xml_parer: missing action 'interval' [action:%s, config:%s, file:%s]",
				a_name, c_name, filename);
	}
	// interval_count
	e = ezxml_child(xml, "interval_count");
	if (e && e->txt) {
		action->interval_count = atoi(e->txt);
	}
	else {
		echo.f("xml_parer: missing action 'interval_count' [action:%s, config:%s, file:%s]",
				a_name, c_name, filename);
	}
	// channel
	e = ezxml_child(xml, "channel");
	if (e && e->txt) {
		action->channel = (u8)atoi(e->txt);
	}
	else {
		echo.f("xml_parer: missing action 'channel' [action:%s, config:%s, file:%s]",
				a_name, c_name, filename);
	}

	// shooter
	e = ezxml_child(xml, "shooter");
	if (e) {
		action->shooter = xml_shooter_detail_parsing(action ,e);
	}

	/*********************************************
	 *
	 * TODO:  mandatory <shooter> ?????
	 *
	 *********************************************/
	if (!action->shooter) {
		echo.f("xml_parer: missing <shooter> [action:%s, config:%s, file:%s]",
				a_name, c_name, filename);
	}

	// capture
	for (e = ezxml_child(xml, "capture"); e; e = e->next) {
		tmp = xml_capture_detail_parsing(action, e);

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
	Action_t *a1, *a2;

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
		echo.f("missing config 'name' [file :%s]", CUR_VERSION, filename);
	}
	config->name = strdup(e->txt);

	config->filename = strdup(filename);

	config->action = NULL;
	// multi action in config.
	for (e = ezxml_child(xml, "action"); e; e = e->next) {

		action = xml_action_parsing(config, e);
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
		echo.f("missing config 'action' [file: %s]", filename);
	}

	/*
	 * sort action by 'no'
	 */
	sort_actions(&config->action);

	/*
	 * check duplication action id
	 */
	if (get_action_count(config) > 1) {
		for (a1 = config->action; a1; a1 = a1->next) {
			for (a2 = a1->next; a2; a2 = a2->next) {
				if (a1->id == a2->id) {
					echo.f("duplicate action id '%d' (%s) [file :%s]",
							a2->id, a2->name, filename);
				}
			}
		}
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
	echo.i("init_xml_parser: %s", options);

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
		echo.f("error xml_parser: missing filename: %s", options);
	}

//	echo.i("[xml parser options]");
//	echo.i("filename = %s", xmlfilename);
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

