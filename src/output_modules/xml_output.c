#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "format.h"
#include "parser.h"
#include "log.h"
#include "alloc.h"
#include "output.h"
#include "ezxml/ezxml.h"
#include "h80211_struct.h"
#include "convert.h"

static u32 xid;

static void do_xml_output(Action_t *action, Output_data_t *data)
{
	ezxml_t xml;
	ezxml_t child;
	h80211_hdr_t *h = (h80211_hdr_t *)data->h80211;
	h80211_mgmt_t *m = (h80211_mgmt_t *)data->h80211;

	xid++;

	xml = ezxml_new("capture");
	child = ezxml_add_child(xml, "result", 0);
	ezxml_set_attr(child, "config",  strdup(action->config->name));
	ezxml_set_attr(child, "action",  strdup(action->name));
	ezxml_set_attr(child, "xid",     new_ltoa(xid, 10));
	ezxml_set_attr(child, "sec",     new_ltoa(data->tv.tv_sec, 10));
	ezxml_set_attr(child, "usec",    new_ltoa(data->tv.tv_usec, 10));
	ezxml_set_attr(child, "elapsed", new_ltoa(usec2msec(&diff), 10));
	ezxml_set_attr(child, "pwr",     new_itoa(data->ri->ri_power, 10));
	ezxml_set_attr(child, "type",    new_itoa(GET_WLAN_TYPE(h), 10));
	ezxml_set_attr(child, "subtype", new_itoa(GET_WLAN_SUBTYPE(h), 10));
	ezxml_set_attr(child, "fromds",  new_itoa(GET_WLAN_FROMDS(h), 10));
	ezxml_set_attr(child, "tods",    new_itoa(GET_WLAN_TODS(h), 10));
	ezxml_set_attr(child, "seq",     new_itoa(h->fragseq.seq, 10));
	ezxml_set_attr(child, "addr1",   new_macstr(h->addr.u.an.addr1));
	ezxml_set_attr(child, "addr2",   new_macstr(h->addr.u.an.addr2));
	ezxml_set_attr(child, "addr3",   new_macstr(h->addr.u.an.addr3));

	if (GET_WLAN_TYPE(h) == WLAN_FC_TYPE_DATA &&
			GET_WLAN_FROMDS(h) == 1 && GET_WLAN_TODS(h) == 1) {
		ezxml_set_attr(child, "addr4",   new_macstr(h->addr4));
	}
	if (GET_WLAN_TYPE(h) == WLAN_FC_TYPE_MGMT &&
			GET_WLAN_SUBTYPE(h) == WLAN_FC_STYPE_DEAUTH) {
		ezxml_set_attr(child, "deauth",   new_itoa(ntohs(m->u.deauth.reason), 10));
	}

}

static void init_xml_output(void)
{
	echo.d("init_xml_output");
}

static void done_xml_output(void)
{
	echo.d("done_xml_output");
}

void setup_xml_output_module(void)
{
	Output_operations_t op = {
			.init_output = init_xml_output,
			.do_output = do_xml_output,
			.done_output = done_xml_output,
	};

	register_output_module("xml", &op);
}
