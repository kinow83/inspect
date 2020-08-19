#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "format.h"
#include "match.h"
#include "log.h"
#include "alloc.h"
#include "h80211_struct.h"


#define WIFI_MATCH_NAME "wifi"
#ifdef HAVE_DEBUG
	#define TRACE(fmt,...) echo.D(fmt,##__VA_ARGS__)
#else
	#define TRACE
#endif



static void match_debug(Action_t *action, u8 *h80211, size_t h80211len, struct rx_info *ri)
{
	h80211_hdr_t  *h;
	h80211_mgmt_t *m;

	h = (h80211_hdr_t  *)h80211;
	m = (h80211_mgmt_t *)h80211;


}

static bool match_test_tag(Action_details_t *detail, u8 *h80211tag, u8 *h80211, size_t h80211len)
{
	Tag_t *tag;
	int matched_tag_count, left;
	u8 *pos;
	u8 id, elen;

	matched_tag_count = 0;
	tag = detail->tags;
	while (tag) {
		matched_tag_count++;
		tag = tag->next;
	}

	tag = detail->tags;
	while (tag) {
		left = h80211len - (h80211tag - h80211);
		pos = h80211tag;

		while (left >= 2) {
			id = *pos++;
			elen = *pos++;
			left -= 2;

			// malformed element packet
			if (elen > left) {
				return false;
			}
			if (tag->id == id) {
				matched_tag_count--;
				// check element length
				if (tag->len != elen) {
					return false;
				}
				// check element data
				if (memcmp(tag->data, pos, elen)) {
					return false;
				}
			}
			left -= elen;
			pos  += elen;
		}
		// next 802.11 element
		tag = tag->next;
	}
	// left tag elements
	if (matched_tag_count != 0) {
		return false;
	}

	return true;
}

/*
 * match test for management frame packet
 */
static bool match_test_mgnt(Action_details_t *detail, u8 *h80211, size_t h80211len, struct rx_info *ri)
{
	h80211_mgmt_t *h;
	u8 *h80211tag;

	if (!detail) {
		return false;
	}

	h = (h80211_mgmt_t *)h80211;

	if (cv_enabled(detail->version)) {
		if (detail->version != h->fc.version) {
			TRACE("mismatch version [%d != %d]", detail->version, h->fc.version);
			return false;
		}
	}
	if (cv_enabled(detail->duration)) {
		if (detail->duration != h->duration) {
			TRACE("mismatch duration [%d != %d]", detail->duration, h->duration);
			return false;
		}
	}
	if (cv_enabled(detail->ibss)) {
		switch (h->fc.subtype) {
		case WLAN_FC_STYPE_PROBE_RESP:
			if (detail->ibss != h->u.probe_resp.capability.ibss) {
				TRACE("mismatch ibss [%d != %d]", detail->ibss, h->u.probe_resp.capability.ibss);
				return false;
			}
			break;
		case WLAN_FC_STYPE_BEACON:
			if (detail->ibss != h->u.beacon.capability.ibss) {
				TRACE("mismatch ibss [%d != %d]", detail->ibss, h->u.beacon.capability.ibss);
				return false;
			}
			break;
		case WLAN_FC_STYPE_ASSOC_REQ:
			if (detail->ibss != h->u.assoc_req.capability.ibss) {
				TRACE("mismatch ibss [%d != %d]", detail->ibss, h->u.assoc_req.capability.ibss);
				return false;
			}
			break;
		case WLAN_FC_STYPE_ASSOC_RESP:
			if (detail->ibss != h->u.assoc_resp.capability.ibss) {
				TRACE("mismatch ibss [%d != %d]", detail->ibss, h->u.assoc_resp.capability.ibss);
				return false;
			}
			break;
		case WLAN_FC_STYPE_REASSOC_REQ:
			if (detail->ibss != h->u.reassoc_req.capability.ibss) {
				TRACE("mismatch ibss [%d != %d]", detail->ibss, h->u.reassoc_req.capability.ibss);
				return false;
			}
			break;
		case WLAN_FC_STYPE_REASSOC_RESP:
			if (detail->ibss != h->u.reassoc_resp.capability.ibss) {
				TRACE("mismatch ibss [%d != %d]", detail->ibss, h->u.reassoc_resp.capability.ibss);
				return false;
			}
			break;
		default:
			// not exist ibss field.
			return false;
		}
	}
	if (cv_enabled(detail->protect)) {
		if (detail->protect != h->fc.flags.protect) {
			TRACE("mismatch protect [%d != %d]", detail->protect, h->fc.flags.protect);
			return false;
		}
	}
	if (cv_enabled(detail->type)) {
		if (detail->type != h->fc.type) {
			TRACE("mismatch type [%d != %d]", detail->type, h->fc.type);
			return false;
		}
	}
	if (cv_enabled(detail->subtype)) {
		if (detail->subtype != h->fc.subtype) {
			TRACE("mismatch subtype [%d != %d]", detail->subtype, h->fc.subtype);
			return false;
		}
	}
	if (cv_enabled(detail->tods)) {
		if (detail->tods != h->fc.flags.tods) {
			TRACE("mismatch tods [%d != %d]", detail->tods, h->fc.flags.tods);
			return false;
		}
	}
	if (cv_enabled(detail->fromds)) {
		if (detail->fromds != h->fc.flags.fromds) {
			TRACE("mismatch fromds [%d != %d]", detail->fromds, h->fc.flags.fromds);
			return false;
		}
	}
    /**
     * [DS 주소 필드 정의]
     *                                 4              10             16
     * +--------------+--------------+--------------+--------------+--------------+--------------+
     * | To DS(AP)    | From DS(AP)  |   Address1   |  Address2    |   Address3   |   Address4   |
     * +--------------+--------------+--------------+--------------+--------------+--------------+
     * |      0       |     0        |    DA(STA)   |    SA(STA)   |    BSSID(AP) |              | ADHOC
     * +--------------+--------------+--------------+--------------+--------------+--------------+
     * |      0       |     1        |    DA(STA)   |   BSSID(AP)  |    SA        |              | From AP
     * +--------------+--------------+--------------+--------------+--------------+--------------+
     * |      1       |     0        |   BSSID(AP)  |   SA(STA)    |    DA        |              | To AP
     * +--------------+--------------+--------------+--------------+--------------+--------------+
     * |      1       |     1        |   RA(rx AP)  |   TA(tx AP)  |  DA(rx STA)  |  SA(tx STA)  | WDS
     * +--------------+--------------+--------------+--------------+--------------+--------------+
     *                                 4              10             16
     */

	/**
	 * [관리 MAC 프레임 주소 필드 정의]
	 *                                 4              10             16
	 * +--------------+--------------+--------------+--------------+--------------+--------------+
	 * | To DS(AP)    | From DS(AP)  |   Address1   |  Address2    |   Address3   |   Address4   |
	 * +--------------+--------------+--------------+--------------+--------------+--------------+
	 * |      0       |     0        |    DA        |      SA      |    BSSID     |              |
	 * +--------------+--------------+--------------+--------------+--------------+--------------+
	 * |           probe-req         ff:ff:ff:ff:ff:ff     sta     ff:ff:ff:ff:ff:ff             |
	 * |           assoc-req              bssid            sta          bssid                    |
	 * |        re-assoc-req              bssid            sta          bssid                    |
	 * |            beacon           ff:ff:ff:ff:ff:ff    bssid         bssid                    |
	 * |           probe-resp              sta            bssid         bssid                    |
	 * |           assoc-resp              sta            bssid         bssid                    |
	 * |        re-assoc-resp              sta            bssid         bssid                    |
	 * |       authentication           sta (bssid)    bssid (Sta)      bssid                    |
	 * |      de-authentication         sta (bssid)    bssid (Sta)      bssid                    |
	 * |           action               sta (bssid)    bssid (Sta)      bssid                    |
	 * +--------------+--------------+--------------+--------------+--------------+--------------+
	 * |            ADHOC                 DA               SA           BSSID                    |
	 * +--------------+--------------+--------------+--------------+--------------+--------------+
	 *                                 4              10             16
	 */

	// check any_addr
	if (cv_enabled(detail->any_addr)) {
		if (!mac_cmp(detail->any_addr, h->addr.u.n.addr1)
				&& !mac_cmp(detail->any_addr, h->addr.u.n.addr2)
				&& !mac_cmp(detail->any_addr, h->addr.u.n.addr3)) {
			TRACE("mismatch any_addr ["_MAC_FMT_"]", _MAC_FMT_FILL_(detail->any_addr));
			return false;
		}
	}
	// check da or sa
	else if (cv_enabled(detail->da) || cv_enabled(detail->sa)) {
		if (cv_enabled(detail->da)) {
			if (!mac_cmp(detail->da, h->addr.u.d.da)) {
				TRACE("mismatch da ["_MAC_FMT_" != ["_MAC_FMT_"]",
						_MAC_FMT_FILL_(detail->da), _MAC_FMT_FILL_(h->addr.u.d.da));
				return false;
			}
		}
		if (cv_enabled(detail->sa)) {
			if (!mac_cmp(detail->sa, h->addr.u.d.sa)) {
				TRACE("mismatch sa ["_MAC_FMT_" != ["_MAC_FMT_"]",
						_MAC_FMT_FILL_(detail->sa), _MAC_FMT_FILL_(h->addr.u.d.sa));
				return false;
			}
		}
	}
	else {
		// check addr1
		if (cv_enabled(detail->addr1)) {
			if (!mac_cmp(detail->addr1, h->addr.u.n.addr1)) {
				TRACE("mismatch addr1 ["_MAC_FMT_"]", _MAC_FMT_FILL_(detail->addr1));
				return false;
			}
		}
		// check addr2
		if (cv_enabled(detail->addr2)) {
			if (!mac_cmp(detail->addr2, h->addr.u.n.addr2)) {
				TRACE("mismatch addr2 ["_MAC_FMT_"]", _MAC_FMT_FILL_(detail->addr2));
				return false;
			}
		}
		// check addr3
		if (cv_enabled(detail->addr3)) {
			if (!mac_cmp(detail->addr3, h->addr.u.n.addr3)) {
				TRACE("mismatch addr3 ["_MAC_FMT_"]", _MAC_FMT_FILL_(detail->addr3));
				return false;
			}
		}
	}

	// check deauth reason code when catch deauthentication packet
	if (cv_enabled(detail->deauth_reason)) {
		if (h->fc.subtype != WLAN_FC_STYPE_DEAUTH) {
			TRACE("mismatch this packet is not deauth");
			return false;
		}
		if (detail->deauth_reason != h->u.deauth.reason) {
			TRACE("mismatch deauth_reason [%d != %d]", detail->deauth_reason, h->u.deauth.reason);
			return false;
		}
	}

    /*
     * check length of fixed parameter for tag list
     */
	h80211tag = NULL;
	switch (h->fc.subtype) {
	case WLAN_FC_STYPE_ASSOC_REQ:
		h80211tag = h->u.assoc_req.variable;
		break;
	case WLAN_FC_STYPE_ASSOC_RESP:
		h80211tag = h->u.assoc_resp.variable;
		break;
	case WLAN_FC_STYPE_REASSOC_REQ:
		h80211tag = h->u.reassoc_req.variable;
		break;
	case WLAN_FC_STYPE_REASSOC_RESP:
		h80211tag = h->u.reassoc_resp.variable;
		break;
	case WLAN_FC_STYPE_PROBE_REQ:
		h80211tag = h->u.probe_req.variable;
		break;
	case WLAN_FC_STYPE_PROBE_RESP:
		h80211tag = h->u.probe_resp.variable;
		break;
	case WLAN_FC_STYPE_BEACON:
		h80211tag = h->u.beacon.variable;
		break;
	case WLAN_FC_STYPE_ATIM:
		break;
	case WLAN_FC_STYPE_DISASSOC:
		h80211tag = h->u.disassoc.variable;
		break;
	case WLAN_FC_STYPE_AUTH:
		h80211tag = h->u.auth.variable;
		break;
	case WLAN_FC_STYPE_DEAUTH:
		break;
	case WLAN_FC_STYPE_ACTION:
		break;
	}

	// test matching 802.11 tag list
	if (h80211tag && detail->tags) {
		if (!match_test_tag(detail, h80211tag, h80211, h80211len)) {
			return false;
		}
	}
	return true;
}

/*
 * match test for control frame packet
 */
static bool match_test_ctrl(Action_details_t *detail, u8 *h80211, size_t h80211len, struct rx_info *ri)
{
	return false;
}

/*
 * match test for data frame packet
 */
static bool match_test_data(Action_details_t *detail, u8 *h80211, size_t h80211len, struct rx_info *ri)
{
	return false;
}

static Action_details_t *match_test(Action_t *action, u8 *h80211, size_t h80211len, struct rx_info *ri)
{
	Action_details_t *capture;
	h80211_hdr_t *h;

	h = (h80211_hdr_t *)h80211;

	capture = action->capture;

	/***************************************************************
	 *
	 * shooter(송신한 패킷) 설정이 있는 경우
	 * shooter 패킷이 제대로 송신 되었는지 수신하여 확인해야 한다.
	 *
	 ***************************************************************/
	switch (h->fc.type) {
	// management frame
	case WLAN_FC_TYPE_MGMT:
		if (action->shooter &&
				match_test_mgnt(action->shooter, h80211, h80211len, ri)) {
			return action->shooter;
		}
		while (capture) {
			if (match_test_mgnt(capture, h80211, h80211len, ri)) {
				return capture;
			}
			capture = capture->next;
		}
		break;
	// control frame
	case WLAN_FC_TYPE_CTRL:
		if (action->shooter &&
				match_test_ctrl(action->shooter, h80211, h80211len, ri)) {
			return action->shooter;
		}
		while (capture) {
			if (match_test_ctrl(capture, h80211, h80211len, ri)) {
				return capture;
			}
			capture = capture->next;
		}
		break;
	// data frame
	case WLAN_FC_TYPE_DATA:
		if (action->shooter &&
				match_test_data(action->shooter, h80211, h80211len, ri)) {
			return action->shooter;
		}
		while (capture) {
			if (match_test_data(capture, h80211, h80211len, ri)) {
				return capture;
			}
			capture = capture->next;
		}
		break;
	default:
		break;
	}
	return NULL;
}


static Action_details_t *do_wifi_match(Config_t *config, u8 *h80211, size_t h80211len, struct rx_info *ri)
{
	Action_t *action;
	Action_details_t *matched;

	action = config->action;
	while (action) {
		// test match all configs.
		if (action->enable) {
			matched = match_test(action, h80211, h80211len, ri);
			if (matched) {
				return matched;
			}
		}
		action = action->next;
	}

	return NULL;
}

static void init_wifi_match(char *options)
{
	echo.i("[init_wifi_match]");
}

static void finish_wifi_match()
{
	echo.d("finish_wifi_match");
}

static const char *usage_wifi_match(void)
{
	return "";
}

void setup_wifi_match_module()
{
	Match_operations_t op = {
			.init_match   = init_wifi_match,
			.do_match     = do_wifi_match,
			.finish_match = finish_wifi_match,
			.usage_match  = usage_wifi_match,
	};

	register_match_module(WIFI_MATCH_NAME, &op);
}
