/*
 * wifi_rtx.c
 *
 *  Created on: 2017. 8. 3.
 *      Author: root
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "format.h"
#include "parser.h"
#include "log.h"
#include "match.h"
#include "output.h"
#include "alloc.h"
#include "strings.h"
#include "rtx.h"
#include "h80211_struct.h"
#include "osdep/osdep.h"


typedef struct shooter_packet_t {
	Action_t *action;
	u8 *pkt;
	size_t pktlen;
	struct shooter_packet_t *next;
} shooter_packet_t;


static struct wif *ShooterWif;
static struct wif *CaptureWif;
shooter_packet_t *ShooterPackets;
pthread_t CapturePid;

static struct wif *new_wif(char *options)
{
	char **chunk, **field;
	int nchunk, nfield;
	int i;
	struct wif *wi;

	chunk = new_splits(options, ",", &nchunk);
	if (!chunk || nchunk == 0) {
		echo.f("error wifi_rtx: invalid option: %s", options);
	}
	for (i=0; i<nchunk; i++) {
		field = new_splits(chunk[i], "=", &nfield);
		if (!field || nfield != 2) {
			echo.f("error wifi_rtx: invalid option: %s", chunk[i]);
		}
		if (!strcasecmp("dev", field[0])) {
			wi = wi_open(field[1]);
			if (!wi) {
				echo.f("error wifi_rtx: can't open: %s", field[1]);
			}
		}
		free_splits(field, nfield);
	}
	free_splits(chunk, nchunk);

	return wi;
}

static void init_wifi_capture_rtx(char *options)
{
	CaptureWif = new_wif(options);
	echo.i("[wifi capture rtx options]");
	echo.i("dev = %s", CaptureWif->wi_interface);
}

static void finish_wifi_capture_rtx(void)
{
	if (ShooterWif) {
		wi_close(ShooterWif);
		ShooterWif = NULL;
	}
}

static void xlat_output_data(u8 *h80211, size_t h80211len, struct rx_info *ri, Output_data_t *output)
{
	h80211_hdr_t  *h;
	h80211_mgmt_t *m;

	h = (h80211_hdr_t  *)h80211;
	m = (h80211_mgmt_t *)h80211;
}

static void *do_wifi_capture_rtx_thread(void *arg)
{
	Config_t *config;
	Action_t *action;
	u8 h80211[4096];
	int h80211len;
	struct rx_info ri;
	Output_data_t output;

	config = (Config_t *)arg;

	while (true) {
		h80211len = CaptureWif->wi_read(CaptureWif, h80211, sizeof(h80211), &ri);

		action = do_match(config, h80211, h80211len, &ri);
		if (action) {
			xlat_output_data(h80211, h80211len, &ri, &output);
			do_output(action, &output);
		}
	}
}

static void do_wifi_capture_rtx(Config_t *config)
{
	if (pthread_create(&CapturePid, NULL,
			do_wifi_capture_rtx_thread, (void *)config) != 0) {
		echo.f("do_wifi_capture_rtx_thread: %s", strerror(errno));
	}
	pthread_detach(CapturePid);
}

static const char *usage_wifi_capture_rtx(void)
{
	return "wifi_capture:dev=<ifname>";
}




static void init_wifi_shooter_rtx(char *options)
{
	ShooterWif = new_wif(options);
	echo.i("[wifi shooter rtx options]");
	echo.i("dev = %s", ShooterWif->wi_interface);
}

static void finish_wifi_shooter_rtx(void)
{
	shooter_packet_t *pkts, *tmp;

	if (ShooterWif) {
		wi_close(ShooterWif);
		ShooterWif = NULL;
	}

	pkts = ShooterPackets;
	while (pkts) {
		tmp = pkts->next;
		if (tmp->pkt) {
			free(tmp->pkt);
		}
		free(tmp);

		pkts = tmp;
	}
}

static size_t build_wifi_shooter_packet(Action_t *action, u8 *buf, size_t buflen)
{
	size_t nbytes = 0;

	return nbytes;
}

static void send_wifi_shooter_packet(Action_t *action, u8 *pkt, size_t pktlen)
{
	echo.d("send wifi shooter packet");
}

static void do_wifi_shooter_rtx(Config_t *config)
{
	Action_t *action;
	shooter_packet_t *pkts;

	pkts = ShooterPackets;
	for (action=config->action; action; action=action->next) {
		if (!pkts) {
			ShooterPackets = alloc_sizeof(shooter_packet_t);
			ShooterPackets->action = action;
			ShooterPackets->pkt = alloc_type(u8, 4096);
			ShooterPackets->pktlen = build_wifi_shooter_packet(action, ShooterPackets->pkt, 4096);
		}
		else {
			// reach last
			while (!pkts->next) {
				pkts = pkts->next;
			}

			pkts->next = alloc_sizeof(shooter_packet_t);

			pkts = pkts->next;

			pkts->action = action;
			pkts->pkt = alloc_type(u8, 4096);
			pkts->pktlen = build_wifi_shooter_packet(action, ShooterPackets->pkt, 4096);
		}
	}

	while (true) {
		for (pkts=ShooterPackets; pkts; pkts=pkts->next) {
			send_wifi_shooter_packet(action, pkts->pkt, pkts->pktlen);

			sleep(1);
		}
	}
}

static const char *usage_wifi_shooter_rtx(void)
{
	return "wifi_shooter:dev=<ifname>";
}

void setup_wifi_rtx_module(void)
{
	RTX_operations_t shooter_op = {
		.init_rtx   = init_wifi_shooter_rtx,
		.do_rtx     = do_wifi_shooter_rtx,
		.finish_rtx = finish_wifi_shooter_rtx,
		.usage_rtx  = usage_wifi_shooter_rtx,
	};
	RTX_operations_t capture_op = {
		.init_rtx   = init_wifi_capture_rtx,
		.do_rtx     = do_wifi_capture_rtx,
		.finish_rtx = finish_wifi_capture_rtx,
		.usage_rtx  = usage_wifi_capture_rtx,
	};

	register_rtx_module("wifi_capture", &capture_op);
	register_rtx_module("wifi_shooter", &shooter_op);
}
