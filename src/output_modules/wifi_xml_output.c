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
#include "strings.h"

#define WIFI_XML_OUTPUT_NAME "xml"

static u32 xid;
static char *prefix;

static void do_wifi_xml_output(Action_details_t *detail, void *output_data)
{
	ezxml_t xml;
	Action_t *action;
	Output_wifi_data_t *data;
	h80211_hdr_t *h;
	h80211_mgmt_t *m;

	action = detail->action;

	data = (Output_wifi_data_t *)output_data;
	h = (h80211_hdr_t *)data->h80211;
	m = (h80211_mgmt_t *)h;

	xid++;

	xml = ezxml_new("output");
	ezxml_set_attr(xml, "config",  strdup(action->config->name));
	ezxml_set_attr(xml, "action",  strdup(action->name));
	ezxml_set_attr(xml, "xid",     new_ltoa(xid, 10));
	ezxml_set_attr(xml, "sec",     new_ltoa(data->tv.tv_sec, 10));
	ezxml_set_attr(xml, "usec",    new_ltoa(data->tv.tv_usec, 10));
//	ezxml_set_attr(xml, "elapsed", new_ltoa(usec2msec(&data->tv), 10));
	ezxml_set_attr(xml, "pwr",     new_itoa(data->ri->ri_power, 10));
	ezxml_set_attr(xml, "type",    new_itoa(GET_WLAN_TYPE(h), 10));
	ezxml_set_attr(xml, "subtype", new_itoa(GET_WLAN_SUBTYPE(h), 10));
	ezxml_set_attr(xml, "fromds",  new_itoa(GET_WLAN_FROMDS(h), 10));
	ezxml_set_attr(xml, "tods",    new_itoa(GET_WLAN_TODS(h), 10));
	ezxml_set_attr(xml, "seq",     new_itoa(h->fragseq.seq, 10));
	ezxml_set_attr(xml, "addr1",   new_macstr(h->addr.u.n.addr1));
	ezxml_set_attr(xml, "addr2",   new_macstr(h->addr.u.n.addr2));
	ezxml_set_attr(xml, "addr3",   new_macstr(h->addr.u.n.addr3));

	if (GET_WLAN_TYPE(h) == WLAN_FC_TYPE_DATA &&
			GET_WLAN_FROMDS(h) == 1 && GET_WLAN_TODS(h) == 1) {
		ezxml_set_attr(xml, "addr4",   new_macstr(h->addr4));
	}
	if (GET_WLAN_TYPE(h) == WLAN_FC_TYPE_MGMT &&
			GET_WLAN_SUBTYPE(h) == WLAN_FC_STYPE_DEAUTH) {
		ezxml_set_attr(xml, "deauth",   new_itoa(ntohs(m->u.deauth.reason), 10));
	}

	echo.i("%s", ezxml_toxml(xml));

	ezxml_free(xml);
}

static void init_wifi_xml_output(char *options)
{
	char **chunk, **field;
	int nchunk, nfield;
	int i;

	chunk = new_splits(options, ",", &nchunk);
	if (!chunk || nchunk == 0) {
		echo.f("error xml_output: invalid option: %s", options);
	}
	for (i=0; i<nchunk; i++) {
		field = new_splits(chunk[i], "=", &nfield);
		if (!field || nfield != 2) {
			echo.f("error xml_output: invalid option: %s", chunk[i]);
		}
		// output prefix filename
		if (!strcasecmp("prefix", field[0])) {
			prefix = strdup(field[1]);
		}
		free_splits(field, nfield);
	}
	free_splits(chunk, nchunk);

	if (!prefix || strlen(prefix) == 0) {
		echo.f("error xml_output: missing prefix: %s", options);
	}

	xid = 0;

	echo.i("[xml output options]");
	echo.i("prefix = %s", prefix);
}

static void finish_wifi_xml_output(void)
{
	if (prefix) {
		free(prefix);
		prefix = NULL;
	}
	xid = 0;
}

static const char *usage_wifi_xml_output(void)
{
	return "xml:prefix=<prefix of output file>";
}

void setup_wifi_xml_output_module(void)
{
	Output_operations_t op = {
			.init_output   = init_wifi_xml_output,
			.do_output     = do_wifi_xml_output,
			.finish_output = finish_wifi_xml_output,
			.usage_output  = usage_wifi_xml_output,
	};

	register_output_module(WIFI_XML_OUTPUT_NAME, &op);
}
