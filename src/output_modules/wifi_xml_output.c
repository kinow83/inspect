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


static u32 xid;
static char *prefix;

static void do_wifi_xml_output(Action_t *action, void *output_data)
{
	ezxml_t xml;
	ezxml_t child;
	Output_wifi_data_t *data;
	h80211_hdr_t *h;
	h80211_mgmt_t *m;

	data = (Output_wifi_data_t *)output_data;
	h = (h80211_hdr_t *)data->h80211;
	m = (h80211_mgmt_t *)h;

	xid++;

	xml = ezxml_new("capture");
	child = ezxml_add_child(xml, "result", 0);
	ezxml_set_attr(child, "config",  strdup(action->config->name));
	ezxml_set_attr(child, "action",  strdup(action->name));
	ezxml_set_attr(child, "xid",     new_ltoa(xid, 10));
	ezxml_set_attr(child, "sec",     new_ltoa(data->tv.tv_sec, 10));
	ezxml_set_attr(child, "usec",    new_ltoa(data->tv.tv_usec, 10));
//	ezxml_set_attr(child, "elapsed", new_ltoa(usec2msec(&diff), 10));
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
		echo.f("error xml_output: empty prefix: %s", options);
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

	register_output_module("xml", &op);
}
