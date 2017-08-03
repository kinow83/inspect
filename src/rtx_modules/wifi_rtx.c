/*
 * wifi_rtx.c
 *
 *  Created on: 2017. 8. 3.
 *      Author: root
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "format.h"
#include "parser.h"
#include "log.h"
#include "alloc.h"
#include "strings.h"
#include "rtx.h"
#include "osdep/osdep.h"


typedef struct shooter_packet_t {
	Action_t *action;
	u8 *pkt;
	size_t pktlen;
	struct shooter_packet_t *next;
} shooter_packet_t;


static struct wif *shooter_wif;
shooter_packet_t *ShooterPackets;

static void init_wifi_shooter_rtx(char *options)
{
	echo.d("init_wifi_shooter_rtx: %s", options);

	char **chunk, **field;
	int nchunk, nfield;
	int i;

	chunk = new_splits(options, ",", &nchunk);
	if (!chunk || nchunk == 0) {
		echo.f("error wifi_shooter_rtx: invalid option: %s", options);
	}
	for (i=0; i<nchunk; i++) {
		field = new_splits(chunk[i], "=", &nfield);
		if (!field || nfield != 2) {
			echo.f("error wifi_shooter_rtx: invalid option: %s", chunk[i]);
		}
		if (!strcasecmp("dev", field[0])) {
			if (shooter_wif) {
				echo.f("error wifi_shooter_rtx: multi define dev: %s", options);
			}
			else {
				shooter_wif = wi_open(field[1]);
				if (!shooter_wif) {
					echo.f("error wifi_shooter_rtx: can't open: %s", field[1]);
				}
			}
		}
		free_splits(field, nfield);
	}
	free_splits(chunk, nchunk);


	echo.i("[wifi shooter rtx options]");
	echo.i("dev = %s", shooter_wif->wi_interface);
}

static void finish_wifi_shooter_rtx(void)
{
	shooter_packet_t *pkts, *tmp;

	echo.d("finish_wifi_shooter_rtx");

	if (shooter_wif) {
		wi_close(shooter_wif);
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
	register_rtx_module("wifi_shooter", &shooter_op);
}
