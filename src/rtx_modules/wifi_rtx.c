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
#include <sys/time.h>
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



#define WIFI_CAPTURE_NAME "wifi_capture"
#define WIFI_SHOOTER_NAME "wifi_shooter"

/*
 * 1,000     msec => 1 sec
 * 1,000,000 usec => 1 sec
 *
 * for capture delay time
 * wait 10,000 msec (0.01 sec)
 */
#define CAPTURE_GUARD_INTERVAL 10000

extern bool do_exit;

static Wif_args_t *ShooterWif;
static Wif_args_t *CaptureWif;
shooter_packet_t *ShooterPackets;
static RTX_module_t *ShooterRTXModule;
static RTX_module_t *CaptureRTXModule;



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
	for (i = 0; i < nchunk; i++) {
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
	echo.i("thread = %d", CaptureWif->thread);
}

static void finish_wifi_capture_rtx(void)
{
	if (CaptureWif) {
		if (CaptureWif->wi) {
			echo.d("finish_wifi_capture_rtx: close wi: %s", CaptureWif->wi->wi_interface);
			wi_close(CaptureWif->wi);
			CaptureWif->wi = NULL;
		}
	}
}

static void xlat_output_data(u8 *h80211, int h80211len, struct rx_info *ri, Output_wifi_data_t *output)
{
	output->h80211 = h80211;
	output->h80211len = h80211len;
	output->ri = ri;
	gettimeofday(&output->tv, NULL);
}

static void *do_wifi_capture_rtx_func(void *arg)
{
	Config_t *config;
	Action_details_t *detail;
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

	/*
	 * FIXME: because of thread environment, MUST finish control
	 */
	CaptureRTXModule->finished = false;

	while (likely(!do_exit)) {

		h80211len = wi_read(CaptureWif->wi, h80211, sizeof(h80211), &ri);

		detail = do_match_modules(config, h80211, h80211len, &ri);
		if (detail) {
			// translate h80211 packet to wifi output
			xlat_output_data(h80211, h80211len, &ri, &output);
			do_output(detail, &output);
		}

	}

	/*
	 * FIXME: because of thread environment, MUST finish control
	 */
	CaptureRTXModule->finished = true;

	return NULL;
}

static void do_wifi_capture_rtx(Config_t *config, RTX_module_t *module)
{
	CaptureRTXModule = module;
	CaptureWif->pid = run_or_thread(
			config, CaptureWif->thread, do_wifi_capture_rtx_func);
}

static const char *usage_wifi_capture_rtx(void)
{
	return "wifi_capture:dev=<ifname>;thread=<yes/no>";
}



/****************************************
 * wifi shooter modules
 ****************************************/
static void init_wifi_shooter_rtx(char *options)
{
	ShooterWif = new_wif(options);

	echo.i("[wifi shooter rtx options]");
	echo.i("dev = %s", ShooterWif->wi->wi_interface);
	echo.i("thread = %d", ShooterWif->thread);
}

static void finish_wifi_shooter_rtx(void)
{
	shooter_packet_t *pkts, *tmp;

	if (ShooterWif) {
		if (ShooterWif->wi) {
			echo.d("finish_wifi_shooter_rtx: close wi: %s", ShooterWif->wi->wi_interface);
			wi_close(ShooterWif->wi);
			ShooterWif->wi = NULL;
		}
	}

	pkts = ShooterPackets;
	while (pkts) {
		tmp = pkts->next;
		if (pkts->pkt) {
			free(pkts->pkt);
		}
		free(pkts);

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

	echo.i("build: config:%s, action:%s", detail->action->config->name, detail->action->name);

	return nbytes;
}

static void *do_wifi_shooter_rtx_func(void *arg)
{
	Config_t *config;
	Action_t *action;
	shooter_packet_t *pkts;
	int nbytes;
	u32 count, prev_channel;
	static const int max_pktlen = 4096;

	config = (Config_t *)arg;

	/*
	 * FIXME: because of thread environment, MUST finish control
	 */
	ShooterRTXModule->finished = false;
	/**
	 * prepare build shooter packet.
	 */
	pkts = ShooterPackets;
	for (action = config->action; action; action = action->next) {
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
	prev_channel = 0;
#if 0
	while (likely(!do_exit)) {
#endif

		for (pkts = ShooterPackets; pkts; pkts = pkts->next) {

			/************************************************************************
			 *
			 * TODO: switch wifi channel
			 * shooter 모듈이 채널 변경 (channel switch) 권한을 가지고 있다.
			 *
			 ************************************************************************/
			wi_set_channel(ShooterWif->wi, pkts->action->channel);
			if (CaptureWif) {
				wi_set_channel(CaptureWif->wi, pkts->action->channel);
			}
			echo.d("switch channel: [%d] -> [%d]", prev_channel, pkts->action->channel);
			prev_channel = pkts->action->channel;


			for (count = 0; count < pkts->action->interval_count; count++) {

				//nbytes = wi_write(ShooterWif->wi, pkts->pkt, pkts->pktlen, NULL);

				usleep(pkts->action->interval + CAPTURE_GUARD_INTERVAL);
			}

		}

#if 0
	}
#endif

	/*
	 * FIXME: because of thread environment, MUST finish control
	 */
	ShooterRTXModule->finished = true;

	return NULL;
}

static void do_wifi_shooter_rtx(Config_t *config, RTX_module_t *module)
{
	ShooterRTXModule = module;
	ShooterWif->pid = run_or_thread(
			config, ShooterWif->thread, do_wifi_shooter_rtx_func);
}

static const char *usage_wifi_shooter_rtx(void)
{
	return "wifi_shooter:dev=<ifname>;thread=<yes/no>";
}

void setup_wifi_rtx_module(void)
{
	/**
	 * TODO: capture 모듈을 먼저 등록하는 것이 좋다.
	 * 먼저 등록한 모듈이 먼저 실행되기 때문에
	 * capture 모듈이 먼저 수행 한 후 에
	 * shooter 모듈이 수행하면 좀 더 정확할 거 같다
	 */

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

	CaptureWif = NULL;
	ShooterWif = NULL;
	ShooterPackets = NULL;
}
