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
#include "string_util.h"
#include "rtx.h"
#include "match.h"
#include "h80211_struct.h"
#include "osdep/osdep.h"

#define RTX_FINISH_CONTRL 1

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

#if 0
	/*
	 * FIXME: because of thread environment, MUST finish control
	 */
	CaptureRTXModule->finished = false;
#endif

	echo.i("do wifi capture start");

	while (likely(!do_exit)) {

		h80211len = wi_read(CaptureWif->wi, h80211, sizeof(h80211), &ri);

		detail = do_match_modules(config, h80211, h80211len, &ri);
		if (detail) {
			// translate h80211 packet to wifi output
			xlat_output_data(h80211, h80211len, &ri, &output);
			do_output(detail, &output);
		}

	}

	echo.e("do wifi capture stop");

#ifdef RTX_FINISH_CONTRL
	/*
	 * FIXME: because of thread environment, MUST finish control
	 */
	CaptureRTXModule->finished = true;
#endif

	return NULL;
}

static void do_wifi_capture_rtx(Config_t *config, RTX_module_t *module)
{
	CaptureRTXModule = module;
	CaptureWif->pid = run_or_thread(
			config, module->rtx_name,
			CaptureWif->thread, do_wifi_capture_rtx_func);
}

static const char *usage_wifi_capture_rtx(void)
{
	return "wifi_capture:dev=<ifname>,thread=<yes/no>";
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

Action_details_t DefaultActionDetail = {
		.version = 0,
		.ibss = 0,
		.protect = 0,
		.duration = 0,
		.type = 0,
		.subtype = 0,
		.tods = 0,
		.fromds = 0,
		.addr1 = {0x00, },
		.addr2 = {0x00, },
		.addr3 = {0x00, },
		.addr4 = {0x00, },
		.da = {0x00, },
		.sa = {0x00, },
		.any_addr = {0x00, },
		.deauth_reason = 0,
};

static size_t build_tags(Action_details_t *detail, u8 *buf)
{
	Tag_t *tag;
	struct h80211_tag_t *p;
	size_t len;

	len = 0;
	tag = detail->tags;
	while (tag) {
		p = (struct h80211_tag_t *)(buf + len);
		p->id = tag->id;
		p->len = tag->len;
		if (tag->data) {
			memcpy(p->data, tag->data, tag->len);
		} else {
			p->len = 0;
		}
		len += sizeof(struct h80211_tag_t) + p->len;

		tag = tag->next;
	}
	return len;
}

static size_t build_data(Action_details_t *detail, u8 *buf, bool qos, bool data)
{
	int i, len = 0;
	u8 realdata;
	struct h80211_qos *p_qos;
	struct h80211_llc *p_llc;
	time_t t = time(NULL);

	static u8 qos_priority[] = {
			0, // not effect
			1, // background (background)
			2,
			3, // excellent effort (best effort)
			4, // controlled load (video)
			5, // video (video)
			6, // voice (voice)
			7, // network control (voice)
    };
	static u8 rsns[] = {
			0, // wep
			1, // ccmp
			2, // tkip
	};

	/* Qos Control */
	if (qos) {
		p_qos = (struct h80211_qos *)(buf + len);
		memset(p_qos, 0, sizeof(struct h80211_qos));
		/* random priority */
		p_qos->priority = qos_priority[rand() % (sizeof(qos_priority))];
		len += sizeof(*p_qos);
	}
	/* TKIP/CCMP or WEP */
	if (detail->protect) {
		int rsn = rsns[rand() % (sizeof(rsns))];

		if (rsn == 0) {
			timebase_rand(buf + len, 4);
			/* exten iv bit off */
			buf[len - 1 + 4] &= (~0x20);
			len += 4;
		} else if (rsn == 1) {
			memcpy(buf + len, "\x07\x11\x00\x20\x00\x00\x00\x00", 8);
			buf[len + 3] = 0x20;
			len += 8;
		} else if (rsn == 2) {
			memcpy(buf + len, "\x0a\x2a\x00\x20\x00\x00\x00\x00", 8);
			buf[len + 3] = 0x20;
			len += 8;
		}
	}
	else {
		p_llc = (struct h80211_llc *)(buf + len);
		p_llc->dsap = 0xaa;
		p_llc->ssap = 0xaa;
		p_llc->ctrl = 0x03;
		p_llc->oui[0] = 0; // Encapsulated Ethernet
		p_llc->oui[1] = 0;
		p_llc->oui[2] = 0;

		t = t % 3; // random
		if (t == 0) {
			p_llc->pid[0] = 0x88; // 802.1x
			p_llc->pid[1] = 0x8e;
		} else if (t == 1) {
			p_llc->pid[0] = 0x08; // IPv4
			p_llc->pid[1] = 0x00;
		} else if (t == 2) {
			p_llc->pid[0] = 0x08; // ARP
			p_llc->pid[1] = 0x06;
		}
		len += sizeof(*p_llc);
	}
	if (data) {
		/* FILL random data */
		realdata = buf + len;

		sign_len = strlen(conf->data_sign);
		if ((sign_len > 0) && (sign_len <= conf->data_size)) {
			memcpy(realdata, conf->data_sign, sign_len);
		}
		for (i = sign_len; i < conf->data_size - sign_len; i++) {
			realdata[i] = rand() & 0xFF;
		}
		len += conf->data_size;
	}

}

static int build_wifi_shooter_packet(
		Action_details_t *detail, u8 *buf, int buflen)
{
	int nbytes = 0;
	h80211_hdr_t *h;
	h80211_mgmt_t *m;
	u8 *p_tag;

	p_tag = NULL;
	h = (h80211_hdr_t *)buf;
	m = (h80211_mgmt_t *)h;

	// version
	if (cv_enabled(detail->version)) {
		h->fc.version = detail->version;
	} else {
		h->fc.version = DefaultActionDetail.version;
	}
	// ibss
	if (cv_enabled(detail->type) && cv_enabled(detail->subtype)) {
		if (detail->type == WLAN_FC_TYPE_MGMT) {
			switch (detail->subtype) {
			/*
			 * deauth_reason
			 */
			case WLAN_FC_STYPE_DEAUTH:
				if (cv_enabled(detail->deauth_reason)) {
					m->u.deauth.reason = htons(detail->deauth_reason);
				} else {
					m->u.deauth.reason = htons(DefaultActionDetail.deauth_reason);
				}
				break;
			case WLAN_FC_STYPE_PROBE_RESP:
				if (cv_enabled(detail->ibss)) {
					m->u.probe_resp.capability.ibss = detail->ibss;
				} else {
					m->u.probe_resp.capability.ibss = DefaultActionDetail.ibss;
				}
				break;
			case WLAN_FC_STYPE_BEACON:
				if (cv_enabled(detail->ibss)) {
					m->u.beacon.capability.ibss = detail->ibss;
				} else {
					m->u.beacon.capability.ibss = DefaultActionDetail.ibss;
				}
				break;
			case WLAN_FC_STYPE_ASSOC_REQ:
				if (cv_enabled(detail->ibss)) {
					m->u.assoc_req.capability.ibss = detail->ibss;
				} else {
					m->u.assoc_req.capability.ibss = DefaultActionDetail.ibss;
				}
				break;
			case WLAN_FC_STYPE_ASSOC_RESP:
				if (cv_enabled(detail->ibss)) {
					m->u.assoc_resp.capability.ibss = detail->ibss;
				} else {
					m->u.assoc_resp.capability.ibss = DefaultActionDetail.ibss;
				}
				break;
			case WLAN_FC_STYPE_REASSOC_REQ:
				if (cv_enabled(detail->ibss)) {
					m->u.reassoc_req.capability.ibss = detail->ibss;
				} else {
					m->u.reassoc_req.capability.ibss = DefaultActionDetail.ibss;
				}
				break;
			case WLAN_FC_STYPE_REASSOC_RESP:
				if (cv_enabled(detail->ibss)) {
					m->u.reassoc_resp.capability.ibss = detail->ibss;
				} else {
					m->u.reassoc_resp.capability.ibss = DefaultActionDetail.ibss;
				}
				break;
			}
		}
	}
	// protect
	if (cv_enabled(detail->protect)) {
		h->fc.flags.protect = detail->protect;
	} else {
		h->fc.flags.protect = DefaultActionDetail.protect;
	}
	// duration
	if (cv_enabled(detail->duration)) {
		h->duration = ntohs(detail->duration);
	} else {
		h->duration = DefaultActionDetail.duration;
	}
	// type
	if (cv_enabled(detail->type)) {
		h->fc.type = detail->type;
	} else {
		h->fc.type = DefaultActionDetail.type;
	}
	// subtype
	if (cv_enabled(detail->subtype)) {
		h->fc.subtype = detail->subtype;
	} else {
		h->fc.subtype = DefaultActionDetail.subtype;
	}
	// fromds
	if (cv_enabled(detail->fromds)) {
		h->fc.flags.fromds = detail->fromds;
	} else {
		h->fc.flags.fromds = DefaultActionDetail.fromds;
	}
	// tods
	if (cv_enabled(detail->tods)) {
		h->fc.flags.tods = detail->tods;
	} else {
		h->fc.flags.tods = DefaultActionDetail.tods;
	}
	// addr1
	if (cv_enabled(detail->addr1)) {
		mac_copy(h->addr.u.n.addr1, detail->addr1);
	} else {
		mac_copy(h->addr.u.n.addr1, DefaultActionDetail.addr1);
	}
	// addr2
	if (cv_enabled(detail->addr2)) {
		mac_copy(h->addr.u.n.addr2, detail->addr2);
	} else {
		mac_copy(h->addr.u.n.addr2, DefaultActionDetail.addr2);
	}
	// addr3
	if (cv_enabled(detail->addr3)) {
		mac_copy(h->addr.u.n.addr3, detail->addr3);
	} else {
		mac_copy(h->addr.u.n.addr3, DefaultActionDetail.addr3);
	}
	// addr4
	if (cv_enabled(detail->addr4)) {
		mac_copy(h->addr4, detail->addr4);
	} else {
		mac_copy(h->addr4, DefaultActionDetail.addr4);
	}
	// da
	if (cv_enabled(detail->da)) {
		mac_copy(h->addr.u.d.da, detail->da);
	} else {
		mac_copy(h->addr.u.d.da, DefaultActionDetail.da);
	}
	// sa
	if (cv_enabled(detail->sa)) {
		mac_copy(h->addr.u.d.sa, detail->sa);
	} else {
		mac_copy(h->addr.u.d.sa, DefaultActionDetail.sa);
	}


	switch (detail->type) {
	case WLAN_FC_TYPE_MGMT:

		switch (detail->subtype) {
		case WLAN_FC_STYPE_ASSOC_REQ:
			/* Transmitter is an AP */
			m->u.assoc_req.capability.ess_capa = 1;
			m->u.assoc_req.capability.privacy = h->fc.flags.protect;
			m->u.assoc_req.capability.preamble = 1;
			m->u.assoc_req.listen_int = htons(10);
			p_tag = m->u.assoc_req.variable;
			break;
		case WLAN_FC_STYPE_ASSOC_RESP:
		{
			static u16 assoc_id = 1;
			/* Transmitter is an AP */
			m->u.assoc_resp.capability.ess_capa = 1;
			m->u.assoc_resp.capability.privacy = h->fc.flags.protect;
			m->u.assoc_resp.capability.preamble = 1;
			m->u.assoc_resp.status = 0;
			m->u.assoc_resp.assoc_id = assoc_id++;
			p_tag = m->u.assoc_resp.variable;
			break;
		}
		case WLAN_FC_STYPE_REASSOC_REQ:
		{
			static struct timeval tv;
			/* Transmitter is an AP */
			m->u.reassoc_req.capability.ess_capa = 1;
			m->u.reassoc_req.capability.privacy = h->fc.flags.protect;
			m->u.reassoc_req.capability.preamble = 1;
			m->u.reassoc_req.listen_int = htons(10);

			/*
			 * random current AP
			 */
			gettimeofday(&tv, NULL);
			m->u.reassoc_req.cur_ap[0] = 0;
			m->u.reassoc_req.cur_ap[1] = (tv.tv_sec) & 0xFF;
			m->u.reassoc_req.cur_ap[2] = (tv.tv_sec >> 8) & 0xFF;
			m->u.reassoc_req.cur_ap[3] = (tv.tv_sec >> 16) & 0xFF;
			m->u.reassoc_req.cur_ap[4] = (tv.tv_usec) & 0xFF;
			m->u.reassoc_req.cur_ap[5] = (tv.tv_usec >> 8) & 0xFF;
			p_tag = m->u.reassoc_req.variable;
			break;
		}
		case WLAN_FC_STYPE_REASSOC_RESP:
			/* Transmitter is an AP */
			m->u.reassoc_resp.capability.ess_capa = 1;
			m->u.reassoc_resp.capability.privacy = h->fc.flags.protect;
			m->u.reassoc_resp.capability.preamble = 1;
			p_tag = m->u.reassoc_resp.variable;
			break;
		case WLAN_FC_STYPE_PROBE_REQ:
			p_tag = m->u.probe_req.variable;
			break;
		case WLAN_FC_STYPE_PROBE_RESP:
		{
			static struct timeval tv;
			gettimeofday(&tv, NULL);

			m->u.probe_resp.timestamp = (tv.tv_sec << 32) + tv.tv_usec;
			m->u.probe_resp.beacon_int = 0x0064;

			/* Transmitter is an AP */
			m->u.probe_resp.capability.ess_capa = 1;
			/* RSN */
			if (find_tag(detail->tags, 48)) {
				m->u.probe_resp.capability.privacy = 1;
			}
			/* WPA */
			if (find_tag_vendor(detail->tags, (u8 *)"\x00\x50\xf2")) {
				m->u.probe_resp.capability.privacy = 1;
			}
			/* HT Capability, HT Information */
			if (find_tag(detail->tags, 45)
					|| find_tag(detail->tags, 61)) {
				m->u.probe_resp.capability.preamble = 0;
			} else {
				m->u.probe_resp.capability.preamble = 1;
			}
			p_tag = m->u.probe_resp.variable;
			break;
		}
		case WLAN_FC_STYPE_BEACON:
		{
			static struct timeval tv;
			gettimeofday(&tv, NULL);

			m->u.beacon.timestamp = (tv.tv_sec << 32) + tv.tv_usec;
			m->u.beacon.beacon_int = 0x0064;

			/* Transmitter is an AP */
			m->u.beacon.capability.ess_capa = 1;
			/* RSN */
			if (find_tag(detail->tags, 48)) {
				m->u.beacon.capability.privacy = 1;
			}
			/* WPA */
			if (find_tag_vendor(detail->tags, (u8 *)"\x00\x50\xf2")) {
				m->u.beacon.capability.privacy = 1;
			}
			/* HT Capability, HT Information */
			if (find_tag(detail->tags, 45)
					|| find_tag(detail->tags, 61)) {
				m->u.beacon.capability.preamble = 0;
			} else {
				m->u.beacon.capability.preamble = 1;
			}
			p_tag = m->u.beacon.variable;
			break;
		}
		case WLAN_FC_STYPE_ATIM:
			break;
		case WLAN_FC_STYPE_DISASSOC:
			break;
		case WLAN_FC_STYPE_AUTH:
			break;
		case WLAN_FC_STYPE_DEAUTH:
			break;
		case WLAN_FC_STYPE_ACTION:
			break;
		}
		break;
	case WLAN_FC_TYPE_CTRL:
		break;
	case WLAN_FC_TYPE_DATA:
		break;
	}



	echo.i("success build: len=%d, detail:%s, action:%s, config:%s",
			nbytes,
			detail->action->config->name,
			detail->action->name,
			detail->action->config->name);

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

#if 0
	/*
	 * FIXME: because of thread environment, MUST finish control
	 */
	ShooterRTXModule->finished = false;
#endif

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

	echo.i("do wifi shooter start");

	for (pkts = ShooterPackets;
			pkts && likely(!do_exit);
			pkts = pkts->next) {

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


		for (count = 0;
				count < pkts->action->interval_count && likely(!do_exit);
				count++) {
			nbytes = wi_write(ShooterWif->wi, pkts->pkt, pkts->pktlen, NULL);
			echo.d("shooter len=%d", nbytes);
			usleep(pkts->action->interval + CAPTURE_GUARD_INTERVAL);

		}

	}

	echo.d("do wifi shooter stop");

#ifdef RTX_FINISH_CONTRL
	/*
	 * FIXME: because of thread environment, MUST finish control
	 */
	ShooterRTXModule->finished = true;
#endif

	return NULL;
}

static void do_wifi_shooter_rtx(Config_t *config, RTX_module_t *module)
{
	ShooterRTXModule = module;
	ShooterWif->pid = run_or_thread(
			config, module->rtx_name,
			ShooterWif->thread, do_wifi_shooter_rtx_func);
}

static const char *usage_wifi_shooter_rtx(void)
{
	return "wifi_shooter:dev=<ifname>,thread=<yes/no>";
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
