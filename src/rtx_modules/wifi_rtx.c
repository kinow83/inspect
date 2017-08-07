/*
 * wifi_rtx.c
 *
 *  Created on: 2017. 8. 3.
 *      Author: root
 */

/**
 * wifi receive and transmit module.
 * wifi-shooter module is not thread.
 * wifi-shooter can change channel.
 * wifi-capture is thread.
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include "format.h"
#include "parser.h"
#include "log.h"
#include "match.h"
#include "output.h"
#include "alloc.h"
#include "strings.h"
#include "rtx.h"
#include "match.h"
#include "h80211_struct.h"
#include "osdep/osdep.h"

#define WIFI_CAPTURE_NAME "wifi_capture"
#define WIFI_SHOOTER_NAME "wifi_shooter"

extern bool do_exit;

typedef struct shooter_packet_t {
	Action_t *action;
	u8 *pkt;
	int pktlen;
	struct shooter_packet_t *next;
} shooter_packet_t;

typedef struct Wif_args_t {
	struct wif *wi;
	bool thread;
	pthread_t pid;
} Wif_args_t;

static Wif_args_t *ShooterWif;
static Wif_args_t *CaptureWif;
shooter_packet_t *ShooterPackets;



static Wif_args_t *new_wif(char *options)
{
	char **chunk, **field;
	int nchunk, nfield;
	int i;
	Wif_args_t *wiarg;

	wiarg = alloc_sizeof(Wif_args_t);
	wiarg->pid = 0;
	wiarg->thread = false;

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
			wiarg->wi = wi_open(field[1]);
			if (!wiarg->wi) {
				echo.f("error wifi_rtx: can't open: %s", field[1]);
			}
		}
		if (!strcasecmp("thread", field[0])) {
			if (!strcasecmp("yes", field[1])
					|| !strcasecmp("1", field[1])
					|| !strcasecmp("true", field[1])) {
				wiarg->thread = true;
			}
		}
		free_splits(field, nfield);
	}
	free_splits(chunk, nchunk);

	if (!wiarg->wi) {
		echo.f("MUST need wifi_rtx interface");
	}

	return wiarg;
}

/****************************************
 * wifi capture modules
 ****************************************/
static void init_wifi_capture_rtx(char *options)
{
	CaptureWif = new_wif(options);

	echo.i("[wifi capture rtx options]");
	echo.i("dev = %s", CaptureWif->wi->wi_interface);
}

static void finish_wifi_capture_rtx(void)
{
	if (CaptureWif) {
		if (CaptureWif->wi) {
			wi_close(CaptureWif->wi);
			CaptureWif->wi = NULL;
		}
	}
}

static void xlat_output_data(u8 *h80211, int h80211len, struct rx_info *ri, Output_wifi_data_t *output)
{
	h80211_hdr_t  *h;
	h80211_mgmt_t *m;

	h = (h80211_hdr_t  *)h80211;
	m = (h80211_mgmt_t *)h80211;



}

static void *do_wifi_capture_rtx_func(void *arg)
{
	Config_t *config;
	Action_t *action;
	u8 h80211[4096];
	int h80211len;
	struct rx_info ri;
	Output_wifi_data_t output;

	config = (Config_t *)arg;

	if (num_enabled_match_modules() == 0) {
		echo.E("wifi capture dosen't have enabled match modules");
		return NULL;
	}
	if (!CaptureWif->wi) {
		echo.F("yet, wifi interface doesn't prepared");
	}

	while (likely(!do_exit)) {
		h80211len = wi_read(CaptureWif->wi, h80211, sizeof(h80211), &ri);

		action = do_match(config, h80211, h80211len, &ri);
		if (action) {
			xlat_output_data(h80211, h80211len, &ri, &output);
			do_output(action, &output);
		}
	}
	mark_finished_rtx_module(WIFI_CAPTURE_NAME);

	return NULL;
}

static void do_wifi_capture_rtx(Config_t *config)
{
	CaptureWif->pid = run_or_thread(
			config, CaptureWif->thread, do_wifi_capture_rtx_func);
}

static const char *usage_wifi_capture_rtx(void)
{
	return "wifi_capture:dev=<ifname>";
}



/****************************************
 * wifi shooter modules
 ****************************************/
static void init_wifi_shooter_rtx(char *options)
{
	ShooterWif = new_wif(options);

	echo.i("[wifi shooter rtx options]");
	echo.i("dev = %s", ShooterWif->wi->wi_interface);
}

static void finish_wifi_shooter_rtx(void)
{
	shooter_packet_t *pkts, *tmp;

	if (ShooterWif) {
		if (ShooterWif->wi) {
			wi_close(ShooterWif->wi);
			ShooterWif->wi = NULL;
		}
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

static int build_wifi_shooter_packet(Action_details_t *detail, u8 *buf, int buflen)
{
	int nbytes = 0;

	h80211_hdr_t *h;
	h80211_mgmt_t *m;

	h = (h80211_hdr_t *)buf;
	m = (h80211_mgmt_t *)h;

	h->fc.version = detail->version;
	h->fc.type = detail->type;
	h->fc.subtype = detail->subtype;
	h->duration = ntohs(detail->duration);
	h->fc.flags.protect = detail->protect;
	if (detail->protect) {
		h->fc.flags.fromds = detail->fromds;
		h->fc.flags.tods = detail->tods;
	}


	return nbytes;
}

static void *do_wifi_shooter_rtx_func(void *arg)
{
	Config_t *config;
	Action_t *action;
	shooter_packet_t *pkts;
	int nbytes;
	static const int max_pktlen = 4096;

	config = (Config_t *)arg;

	/**
	 * prepare build shooter packet.
	 */
	pkts = ShooterPackets;
	for (action=config->action; action; action=action->next) {
		if (action->shooter) {
			if (!pkts) {
				ShooterPackets = alloc_sizeof(shooter_packet_t);
				ShooterPackets->action = action;
				ShooterPackets->pkt = alloc_type(u8, max_pktlen);
				ShooterPackets->pktlen = build_wifi_shooter_packet(
						action->shooter, ShooterPackets->pkt, max_pktlen);
			}
			else {
				while (!pkts->next) {
					pkts = pkts->next;
				}

				pkts->next = alloc_sizeof(shooter_packet_t);
				pkts = pkts->next;
				pkts->action = action;
				pkts->pkt = alloc_type(u8, 4096);
				pkts->pktlen = build_wifi_shooter_packet(
						action->shooter, ShooterPackets->pkt, 4096);
			}
		}
	}

	/**
	 * loop shoot packet
	 */
	while (likely(!do_exit)) {
		for (pkts=ShooterPackets; pkts; pkts=pkts->next) {
			nbytes = wi_write(ShooterWif->wi, pkts->pkt, pkts->pktlen, NULL);
			sleep(1);
		}
	}

	return NULL;
}

static void do_wifi_shooter_rtx(Config_t *config)
{
	ShooterWif->pid = run_or_thread(
			config, ShooterWif->thread, do_wifi_shooter_rtx_func);
}

static const char *usage_wifi_shooter_rtx(void)
{
	return "wifi_shooter:dev=<ifname>";
}

void setup_wifi_rtx_module(void)
{
	RTX_operations_t capture_op = {
		.init_rtx   = init_wifi_capture_rtx,
		.do_rtx     = do_wifi_capture_rtx,
		.finish_rtx = finish_wifi_capture_rtx,
		.usage_rtx  = usage_wifi_capture_rtx,
	};
	register_rtx_module(WIFI_CAPTURE_NAME, &capture_op);

	RTX_operations_t shooter_op = {
		.init_rtx   = init_wifi_shooter_rtx,
		.do_rtx     = do_wifi_shooter_rtx,
		.finish_rtx = finish_wifi_shooter_rtx,
		.usage_rtx  = usage_wifi_shooter_rtx,
	};
	register_rtx_module(WIFI_SHOOTER_NAME, &shooter_op);
}
