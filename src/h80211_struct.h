#ifndef _H80211_STRUCT_
#define _H80211_STRUCT_

#include "types.h"

#ifdef __GNUC__
	#define STRUCT_PACKED __attribute__ ((packed))
#else
	#define STRUCT_PACKED
#endif

#ifndef ETH_ALEN
	#define ETH_ALEN 6
#endif
#ifndef ETH_HLEN
	#define ETH_HLEN 14
#endif
#ifndef IFNAMSIZ
	#define IFNAMSIZ 16
#endif
#ifndef ETH_P_ALL
	#define ETH_P_ALL 0x0003
#endif
#ifndef ETH_P_IP
	#define ETH_P_IP 0x0800
#endif
#ifndef ETH_P_80211_ENCAP
	#define ETH_P_80211_ENCAP 0x890d /* TDLS comes under this category */
#endif
#ifndef ETH_P_PAE
	#define ETH_P_PAE 0x888E /* Port Access Entity (IEEE 802.1X) */
#endif /* ETH_P_PAE */
#ifndef ETH_P_EAPOL
	#define ETH_P_EAPOL ETH_P_PAE
#endif /* ETH_P_EAPOL */
#ifndef ETH_P_RSN_PREAUTH
	#define ETH_P_RSN_PREAUTH 0x88c7
#endif /* ETH_P_RSN_PREAUTH */
#ifndef ETH_P_RRB
	#define ETH_P_RRB 0x890D
#endif /* ETH_P_RRB */

/* IEEE 802.11 defines */

#define WLAN_FC_PVER        0x0003
#define WLAN_FC_TODS        0x0100
#define WLAN_FC_FROMDS      0x0200
#define WLAN_FC_MOREFRAG    0x0400
#define WLAN_FC_RETRY       0x0800
#define WLAN_FC_PWRMGT      0x1000
#define WLAN_FC_MOREDATA    0x2000
#define WLAN_FC_ISWEP       0x4000
#define WLAN_FC_ORDER       0x8000

#define WLAN_FC_GET_TYPE(fc)    (((fc) & 0x000c) >> 2)
#define WLAN_FC_GET_STYPE(fc)   (((fc) & 0x00f0) >> 4)

#define WLAN_INVALID_MGMT_SEQ   0xFFFF

#define WLAN_GET_SEQ_FRAG(seq) ((seq) & (BIT(3) | BIT(2) | BIT(1) | BIT(0)))
#define WLAN_GET_SEQ_SEQ(seq) \
    (((seq) & (~(BIT(3) | BIT(2) | BIT(1) | BIT(0)))) >> 4)

#define WLAN_FC_TYPE_MGMT       0
#define WLAN_FC_TYPE_CTRL       1
#define WLAN_FC_TYPE_DATA       2

#define VERIFY_WLAN_FRAME(type) \
	( \
		((type) == WLAN_FC_TYPE_MGMT) || \
		((type) == WLAN_FC_TYPE_CTRL) || \
		((type) == WLAN_FC_TYPE_DATA) \
	)


/* management */
#define WLAN_FC_STYPE_ASSOC_REQ     0
#define WLAN_FC_STYPE_ASSOC_RESP    1
#define WLAN_FC_STYPE_REASSOC_REQ   2
#define WLAN_FC_STYPE_REASSOC_RESP  3
#define WLAN_FC_STYPE_PROBE_REQ     4
#define WLAN_FC_STYPE_PROBE_RESP    5
#define WLAN_FC_STYPE_BEACON        8
#define WLAN_FC_STYPE_ATIM      9
#define WLAN_FC_STYPE_DISASSOC      10
#define WLAN_FC_STYPE_AUTH      11
#define WLAN_FC_STYPE_DEAUTH        12
#define WLAN_FC_STYPE_ACTION        13

#define VERIFY_WLAN_FRAME_MGNT(subtype) \
	( \
		(((subtype) >= WLAN_FC_STYPE_ASSOC_REQ) && ((subtype) <= WLAN_FC_STYPE_PROBE_RESP)) || \
		(((subtype) >= WLAN_FC_STYPE_BEACON)    && ((subtype) <= WLAN_FC_STYPE_ACTION)) \
	)

/* control */
#define WLAN_FC_STYPE_BLOCK_ACK_REQ   8
#define WLAN_FC_STYPE_BLOCK_ACK       9
#define WLAN_FC_STYPE_PSPOLL        10
#define WLAN_FC_STYPE_RTS       11
#define WLAN_FC_STYPE_CTS       12
#define WLAN_FC_STYPE_ACK       13
#define WLAN_FC_STYPE_CFEND     14
#define WLAN_FC_STYPE_CFENDACK      15

#define VERIFY_WLAN_FRAME_CTRL(subtype) \
	( \
		((subtype) >= WLAN_FC_STYPE_BLOCK_ACK_REQ) && ((subtype) <= WLAN_FC_STYPE_CFENDACK) \
	)


/* data */
/*
type  | subtype
--------------------
b0 b1 | b2 b3 b4 b5
--------------------
 1  0 |  0  0  0  0   Data
 1  0 |  0  0  0  1   Data + CF-ACK  [PCF only]
 1  0 |  0  0  1  0   Data + CF-Poll [PCF only]
 1  0 |  0  0  1  1   Data + CF-ACK + CF-Poll [PCF only]
 1  0 |  0  1  0  0   Null (no data)
 1  0 |  0  1  0  1   CF-ACK (no data) [PCF only]
 1  0 |  0  1  1  0   CF-Poll (no data) [PCF only]
 1  0 |  0  1  1  1   CF-ACK + CF-Poll (no data) [PCF only]
 1  0 |  1  0  0  0   QoS Data [HCF]
 1  0 |  1  0  0  1   Qos Data + CF-ACK [HCF]
 1  0 |  1  0  1  0   Qos Data + CF-Poll [HCF]
 1  0 |  1  0  1  1   Qos Data + CF-ACK + CF-Poll [HCF]
 1  0 |  1  1  0  0   Qos Null (no data) [HCF]
 1  0 |  1  1  0  1   Reserved
 1  0 |  1  1  1  0   Qos CF-Poll (no data) [HCF]
 1  0 |  1  1  1  1   Qos CF-ACK + CF-Poll (no data) [HCF]

b5 = 1 (CF-ACK)
b4 = 1 (CF-Poll)
b3 = 1 (no data)
b2 = 1 (Qos data)
*/
#define WLAN_FC_STYPE_DATA      0
#define WLAN_FC_STYPE_DATA_CFACK    1
#define WLAN_FC_STYPE_DATA_CFPOLL   2
#define WLAN_FC_STYPE_DATA_CFACKPOLL    3
#define WLAN_FC_STYPE_NULLFUNC      4
#define WLAN_FC_STYPE_CFACK     5
#define WLAN_FC_STYPE_CFPOLL        6
#define WLAN_FC_STYPE_CFACKPOLL     7
#define WLAN_FC_STYPE_QOS_DATA      8
#define WLAN_FC_STYPE_QOS_DATA_CFACK    9
#define WLAN_FC_STYPE_QOS_DATA_CFPOLL   10
#define WLAN_FC_STYPE_QOS_DATA_CFACKPOLL    11
#define WLAN_FC_STYPE_QOS_NULL      12
#define WLAN_FC_STYPE_QOS_CFPOLL    14
#define WLAN_FC_STYPE_QOS_CFACKPOLL 15

#define VERIFY_WLAN_FRAME_DATA(subtype) \
	( ((subtype) >= WLAN_FC_STYPE_DATA) && ((subtype) <= WLAN_FC_STYPE_QOS_CFACKPOLL) )

/* Authentication algorithms */
#define WLAN_AUTH_OPEN          0
#define WLAN_AUTH_SHARED_KEY        1
#define WLAN_AUTH_FT            2
#define WLAN_AUTH_SAE           3
#define WLAN_AUTH_LEAP          128

#define WLAN_AUTH_CHALLENGE_LEN 128

#define WLAN_CAPABILITY_ESS BIT(0)
#define WLAN_CAPABILITY_IBSS BIT(1)
#define WLAN_CAPABILITY_CF_POLLABLE BIT(2)
#define WLAN_CAPABILITY_CF_POLL_REQUEST BIT(3)
#define WLAN_CAPABILITY_PRIVACY BIT(4)
#define WLAN_CAPABILITY_SHORT_PREAMBLE BIT(5)
#define WLAN_CAPABILITY_PBCC BIT(6)
#define WLAN_CAPABILITY_CHANNEL_AGILITY BIT(7)
#define WLAN_CAPABILITY_SPECTRUM_MGMT BIT(8)
#define WLAN_CAPABILITY_QOS BIT(9)
#define WLAN_CAPABILITY_SHORT_SLOT_TIME BIT(10)
#define WLAN_CAPABILITY_APSD BIT(11)
#define WLAN_CAPABILITY_RADIO_MEASUREMENT BIT(12)
#define WLAN_CAPABILITY_DSSS_OFDM BIT(13)
#define WLAN_CAPABILITY_DELAYED_BLOCK_ACK BIT(14)
#define WLAN_CAPABILITY_IMM_BLOCK_ACK BIT(15)

/* Status codes (IEEE 802.11-2007, 7.3.1.9, Table 7-23) */
#define WLAN_STATUS_SUCCESS 0
#define WLAN_STATUS_UNSPECIFIED_FAILURE 1
#define WLAN_STATUS_TDLS_WAKEUP_ALTERNATE 2
#define WLAN_STATUS_TDLS_WAKEUP_REJECT 3
#define WLAN_STATUS_SECURITY_DISABLED 5
#define WLAN_STATUS_UNACCEPTABLE_LIFETIME 6
#define WLAN_STATUS_NOT_IN_SAME_BSS 7
#define WLAN_STATUS_CAPS_UNSUPPORTED 10
#define WLAN_STATUS_REASSOC_NO_ASSOC 11
#define WLAN_STATUS_ASSOC_DENIED_UNSPEC 12
#define WLAN_STATUS_NOT_SUPPORTED_AUTH_ALG 13
#define WLAN_STATUS_UNKNOWN_AUTH_TRANSACTION 14
#define WLAN_STATUS_CHALLENGE_FAIL 15
#define WLAN_STATUS_AUTH_TIMEOUT 16
#define WLAN_STATUS_AP_UNABLE_TO_HANDLE_NEW_STA 17
#define WLAN_STATUS_ASSOC_DENIED_RATES 18
/* IEEE 802.11b */
#define WLAN_STATUS_ASSOC_DENIED_NOSHORT 19
#define WLAN_STATUS_ASSOC_DENIED_NOPBCC 20
#define WLAN_STATUS_ASSOC_DENIED_NOAGILITY 21
/* IEEE 802.11h */
#define WLAN_STATUS_SPEC_MGMT_REQUIRED 22
#define WLAN_STATUS_PWR_CAPABILITY_NOT_VALID 23
#define WLAN_STATUS_SUPPORTED_CHANNEL_NOT_VALID 24
/* IEEE 802.11g */
#define WLAN_STATUS_ASSOC_DENIED_NO_SHORT_SLOT_TIME 25
#define WLAN_STATUS_ASSOC_DENIED_NO_DSSS_OFDM 26
#define WLAN_STATUS_ASSOC_DENIED_NO_HT 27
#define WLAN_STATUS_R0KH_UNREACHABLE 28
#define WLAN_STATUS_ASSOC_DENIED_NO_PCO 29
/* IEEE 802.11w */
#define WLAN_STATUS_ASSOC_REJECTED_TEMPORARILY 30
#define WLAN_STATUS_ROBUST_MGMT_FRAME_POLICY_VIOLATION 31
#define WLAN_STATUS_UNSPECIFIED_QOS_FAILURE 32
#define WLAN_STATUS_REQUEST_DECLINED 37
#define WLAN_STATUS_INVALID_PARAMETERS 38
/* IEEE 802.11i */
#define WLAN_STATUS_INVALID_IE 40
#define WLAN_STATUS_GROUP_CIPHER_NOT_VALID 41
#define WLAN_STATUS_PAIRWISE_CIPHER_NOT_VALID 42
#define WLAN_STATUS_AKMP_NOT_VALID 43
#define WLAN_STATUS_UNSUPPORTED_RSN_IE_VERSION 44
#define WLAN_STATUS_INVALID_RSN_IE_CAPAB 45
#define WLAN_STATUS_CIPHER_REJECTED_PER_POLICY 46
#define WLAN_STATUS_TS_NOT_CREATED 47
#define WLAN_STATUS_DIRECT_LINK_NOT_ALLOWED 48
#define WLAN_STATUS_DEST_STA_NOT_PRESENT 49
#define WLAN_STATUS_DEST_STA_NOT_QOS_STA 50
#define WLAN_STATUS_ASSOC_DENIED_LISTEN_INT_TOO_LARGE 51
/* IEEE 802.11r */
#define WLAN_STATUS_INVALID_FT_ACTION_FRAME_COUNT 52
#define WLAN_STATUS_INVALID_PMKID 53
#define WLAN_STATUS_INVALID_MDIE 54
#define WLAN_STATUS_INVALID_FTIE 55
#define WLAN_STATUS_GAS_ADV_PROTO_NOT_SUPPORTED 59
#define WLAN_STATUS_NO_OUTSTANDING_GAS_REQ 60
#define WLAN_STATUS_GAS_RESP_NOT_RECEIVED 61
#define WLAN_STATUS_STA_TIMED_OUT_WAITING_FOR_GAS_RESP 62
#define WLAN_STATUS_GAS_RESP_LARGER_THAN_LIMIT 63
#define WLAN_STATUS_REQ_REFUSED_HOME 64
#define WLAN_STATUS_ADV_SRV_UNREACHABLE 65
#define WLAN_STATUS_REQ_REFUSED_SSPN 67
#define WLAN_STATUS_REQ_REFUSED_UNAUTH_ACCESS 68
#define WLAN_STATUS_INVALID_RSNIE 72
#define WLAN_STATUS_ANTI_CLOGGING_TOKEN_REQ 76
#define WLAN_STATUS_FINITE_CYCLIC_GROUP_NOT_SUPPORTED 77
#define WLAN_STATUS_TRANSMISSION_FAILURE 79
#define WLAN_STATUS_REJECTED_WITH_SUGGESTED_BSS_TRANSITION 82
#define WLAN_STATUS_PENDING_ADMITTING_FST_SESSION 86
#define WLAN_STATUS_QUERY_RESP_OUTSTANDING 95
#define WLAN_STATUS_DENIED_WITH_SUGGESTED_BAND_AND_CHANNEL 99
#define WLAN_STATUS_ASSOC_DENIED_NO_VHT 104

/* Reason codes (IEEE 802.11-2007, 7.3.1.7, Table 7-22) */
#define WLAN_REASON_UNSPECIFIED 1
#define WLAN_REASON_PREV_AUTH_NOT_VALID 2
#define WLAN_REASON_DEAUTH_LEAVING 3
#define WLAN_REASON_DISASSOC_DUE_TO_INACTIVITY 4
#define WLAN_REASON_DISASSOC_AP_BUSY 5
#define WLAN_REASON_CLASS2_FRAME_FROM_NONAUTH_STA 6
#define WLAN_REASON_CLASS3_FRAME_FROM_NONASSOC_STA 7
#define WLAN_REASON_DISASSOC_STA_HAS_LEFT 8
#define WLAN_REASON_STA_REQ_ASSOC_WITHOUT_AUTH 9
/* IEEE 802.11h */
#define WLAN_REASON_PWR_CAPABILITY_NOT_VALID 10
#define WLAN_REASON_SUPPORTED_CHANNEL_NOT_VALID 11
/* IEEE 802.11i */
#define WLAN_REASON_INVALID_IE 13
#define WLAN_REASON_MICHAEL_MIC_FAILURE 14
#define WLAN_REASON_4WAY_HANDSHAKE_TIMEOUT 15
#define WLAN_REASON_GROUP_KEY_UPDATE_TIMEOUT 16
#define WLAN_REASON_IE_IN_4WAY_DIFFERS 17
#define WLAN_REASON_GROUP_CIPHER_NOT_VALID 18
#define WLAN_REASON_PAIRWISE_CIPHER_NOT_VALID 19
#define WLAN_REASON_AKMP_NOT_VALID 20
#define WLAN_REASON_UNSUPPORTED_RSN_IE_VERSION 21
#define WLAN_REASON_INVALID_RSN_IE_CAPAB 22
#define WLAN_REASON_IEEE_802_1X_AUTH_FAILED 23
#define WLAN_REASON_CIPHER_SUITE_REJECTED 24
#define WLAN_REASON_TDLS_TEARDOWN_UNREACHABLE 25
#define WLAN_REASON_TDLS_TEARDOWN_UNSPECIFIED 26
/* IEEE 802.11e */
#define WLAN_REASON_DISASSOC_LOW_ACK 34
/* IEEE 802.11s */
#define WLAN_REASON_MESH_PEERING_CANCELLED 52
#define WLAN_REASON_MESH_MAX_PEERS 53
#define WLAN_REASON_MESH_CONFIG_POLICY_VIOLATION 54
#define WLAN_REASON_MESH_CLOSE_RCVD 55
#define WLAN_REASON_MESH_MAX_RETRIES 56
#define WLAN_REASON_MESH_CONFIRM_TIMEOUT 57
#define WLAN_REASON_MESH_INVALID_GTK 58
#define WLAN_REASON_MESH_INCONSISTENT_PARAMS 59
#define WLAN_REASON_MESH_INVALID_SECURITY_CAP 60


/* Information Element IDs */
#define WLAN_EID_SSID 0
#define WLAN_EID_SUPP_RATES 1
#define WLAN_EID_FH_PARAMS 2
#define WLAN_EID_DS_PARAMS 3
#define WLAN_EID_CF_PARAMS 4
#define WLAN_EID_TIM 5
#define WLAN_EID_IBSS_PARAMS 6
#define WLAN_EID_COUNTRY 7
#define WLAN_EID_BSS_LOAD 11
#define WLAN_EID_CHALLENGE 16
/* EIDs defined by IEEE 802.11h - START */
#define WLAN_EID_PWR_CONSTRAINT 32
#define WLAN_EID_PWR_CAPABILITY 33
#define WLAN_EID_TPC_REQUEST 34
#define WLAN_EID_TPC_REPORT 35
#define WLAN_EID_SUPPORTED_CHANNELS 36
#define WLAN_EID_CHANNEL_SWITCH 37
#define WLAN_EID_MEASURE_REQUEST 38
#define WLAN_EID_MEASURE_REPORT 39
#define WLAN_EID_QUITE 40
#define WLAN_EID_IBSS_DFS 41
/* EIDs defined by IEEE 802.11h - END */
#define WLAN_EID_ERP_INFO 42
#define WLAN_EID_HT_CAP 45
#define WLAN_EID_QOS 46
#define WLAN_EID_RSN 48
#define WLAN_EID_EXT_SUPP_RATES 50
#define WLAN_EID_NEIGHBOR_REPORT 52
#define WLAN_EID_MOBILITY_DOMAIN 54
#define WLAN_EID_FAST_BSS_TRANSITION 55
#define WLAN_EID_TIMEOUT_INTERVAL 56
#define WLAN_EID_RIC_DATA 57
#define WLAN_EID_SUPPORTED_OPERATING_CLASSES 59
#define WLAN_EID_EXT_CHANSWITCH_ANN 60
#define WLAN_EID_HT_OPERATION 61
#define WLAN_EID_SECONDARY_CHANNEL_OFFSET 62
#define WLAN_EID_WAPI 68
#define WLAN_EID_TIME_ADVERTISEMENT 69
#define WLAN_EID_RRM_ENABLED_CAPABILITIES 70
#define WLAN_EID_20_40_BSS_COEXISTENCE 72
#define WLAN_EID_20_40_BSS_INTOLERANT 73
#define WLAN_EID_OVERLAPPING_BSS_SCAN_PARAMS 74
#define WLAN_EID_MMIE 76
#define WLAN_EID_SSID_LIST 84
#define WLAN_EID_BSS_MAX_IDLE_PERIOD 90
#define WLAN_EID_TFS_REQ 91
#define WLAN_EID_TFS_RESP 92
#define WLAN_EID_WNMSLEEP 93
#define WLAN_EID_TIME_ZONE 98
#define WLAN_EID_LINK_ID 101
#define WLAN_EID_INTERWORKING 107
#define WLAN_EID_ADV_PROTO 108
#define WLAN_EID_QOS_MAP_SET 110
#define WLAN_EID_ROAMING_CONSORTIUM 111
#define WLAN_EID_MESH_CONFIG 113
#define WLAN_EID_MESH_ID 114
#define WLAN_EID_PEER_MGMT 117
#define WLAN_EID_EXT_CAPAB 127
#define WLAN_EID_AMPE 139
#define WLAN_EID_MIC 140
#define WLAN_EID_CCKM 156
#define WLAN_EID_MULTI_BAND 158
#define WLAN_EID_SESSION_TRANSITION 164
#define WLAN_EID_VHT_CAP 191
#define WLAN_EID_VHT_OPERATION 192
#define WLAN_EID_VHT_EXTENDED_BSS_LOAD 193
#define WLAN_EID_VHT_WIDE_BW_CHSWITCH  194
#define WLAN_EID_VHT_TRANSMIT_POWER_ENVELOPE 195
#define WLAN_EID_VHT_CHANNEL_SWITCH_WRAPPER 196
#define WLAN_EID_VHT_AID 197
#define WLAN_EID_VHT_QUIET_CHANNEL 198
#define WLAN_EID_VHT_OPERATING_MODE_NOTIFICATION 199
#define WLAN_EID_VENDOR_SPECIFIC 221


/* Action frame categories (IEEE 802.11-2007, 7.3.1.11, Table 7-24) */
#define WLAN_ACTION_SPECTRUM_MGMT 0
#define WLAN_ACTION_QOS 1
#define WLAN_ACTION_DLS 2
#define WLAN_ACTION_BLOCK_ACK 3
#define WLAN_ACTION_PUBLIC 4
#define WLAN_ACTION_RADIO_MEASUREMENT 5
#define WLAN_ACTION_FT 6
#define WLAN_ACTION_HT 7
#define WLAN_ACTION_SA_QUERY 8
#define WLAN_ACTION_PROTECTED_DUAL 9
#define WLAN_ACTION_WNM 10
#define WLAN_ACTION_UNPROTECTED_WNM 11
#define WLAN_ACTION_TDLS 12
#define WLAN_ACTION_SELF_PROTECTED 15
#define WLAN_ACTION_WMM 17 /* WMM Specification 1.1 */
#define WLAN_ACTION_FST 18
#define WLAN_ACTION_VENDOR_SPECIFIC 127

/* Public action codes */
#define WLAN_PA_20_40_BSS_COEX 0
#define WLAN_PA_VENDOR_SPECIFIC 9
#define WLAN_PA_GAS_INITIAL_REQ 10
#define WLAN_PA_GAS_INITIAL_RESP 11
#define WLAN_PA_GAS_COMEBACK_REQ 12
#define WLAN_PA_GAS_COMEBACK_RESP 13
#define WLAN_TDLS_DISCOVERY_RESPONSE 14

/* Protected Dual of Public Action frames */
#define WLAN_PROT_DSE_ENABLEMENT 1
#define WLAN_PROT_DSE_DEENABLEMENT 2
#define WLAN_PROT_EXT_CSA 4
#define WLAN_PROT_MEASUREMENT_REQ 5
#define WLAN_PROT_MEASUREMENT_REPORT 6
#define WLAN_PROT_DSE_POWER_CONSTRAINT 8
#define WLAN_PROT_VENDOR_SPECIFIC 9
#define WLAN_PROT_GAS_INITIAL_REQ 10
#define WLAN_PROT_GAS_INITIAL_RESP 11
#define WLAN_PROT_GAS_COMEBACK_REQ 12
#define WLAN_PROT_GAS_COMEBACK_RESP 13

/* SA Query Action frame (IEEE 802.11w/D8.0, 7.4.9) */
#define WLAN_SA_QUERY_REQUEST 0
#define WLAN_SA_QUERY_RESPONSE 1

#define WLAN_SA_QUERY_TR_ID_LEN 2

/* TDLS action codes */
#define WLAN_TDLS_SETUP_REQUEST 0
#define WLAN_TDLS_SETUP_RESPONSE 1
#define WLAN_TDLS_SETUP_CONFIRM 2
#define WLAN_TDLS_TEARDOWN 3
#define WLAN_TDLS_PEER_TRAFFIC_INDICATION 4
#define WLAN_TDLS_CHANNEL_SWITCH_REQUEST 5
#define WLAN_TDLS_CHANNEL_SWITCH_RESPONSE 6
#define WLAN_TDLS_PEER_PSM_REQUEST 7
#define WLAN_TDLS_PEER_PSM_RESPONSE 8
#define WLAN_TDLS_PEER_TRAFFIC_RESPONSE 9
#define WLAN_TDLS_DISCOVERY_REQUEST 10

/* Radio Measurement Action codes */
#define WLAN_RRM_RADIO_MEASUREMENT_REQUEST 0
#define WLAN_RRM_RADIO_MEASUREMENT_REPORT 1
#define WLAN_RRM_LINK_MEASUREMENT_REQUEST 2
#define WLAN_RRM_LINK_MEASUREMENT_REPORT 3
#define WLAN_RRM_NEIGHBOR_REPORT_REQUEST 4
#define WLAN_RRM_NEIGHBOR_REPORT_RESPONSE 5

/* Radio Measurement capabilities (from RRM Capabilities IE) */
/* byte 1 (out of 5) */
#define WLAN_RRM_CAPS_LINK_MEASUREMENT BIT(0)
#define WLAN_RRM_CAPS_NEIGHBOR_REPORT BIT(1)
/* byte 2 (out of 5) */
#define WLAN_RRM_CAPS_LCI_MEASUREMENT BIT(4)
/* byte 5 (out of 5) */
#define WLAN_RRM_CAPS_FTM_RANGE_REPORT BIT(2)


/*
 * IEEE P802.11-REVmc/D5.0, 9.4.2.21.19 (Fine Timing Measurement Range
 * request) - Minimum AP count
 */
#define WLAN_RRM_RANGE_REQ_MAX_MIN_AP 15

/* Timeout Interval Type */
#define WLAN_TIMEOUT_REASSOC_DEADLINE 1
#define WLAN_TIMEOUT_KEY_LIFETIME 2
#define WLAN_TIMEOUT_ASSOC_COMEBACK 3

/* Interworking element (IEEE 802.11u) - Access Network Options */
#define INTERWORKING_ANO_ACCESS_NETWORK_MASK 0x0f
#define INTERWORKING_ANO_INTERNET 0x10
#define INTERWORKING_ANO_ASRA 0x20
#define INTERWORKING_ANO_ESR 0x40
#define INTERWORKING_ANO_UESA 0x80

#define INTERWORKING_ANT_PRIVATE 0
#define INTERWORKING_ANT_PRIVATE_WITH_GUEST 1
#define INTERWORKING_ANT_CHARGEABLE_PUBLIC 2
#define INTERWORKING_ANT_FREE_PUBLIC 3
#define INTERWORKING_ANT_PERSONAL_DEVICE 4
#define INTERWORKING_ANT_EMERGENCY_SERVICES 5
#define INTERWORKING_ANT_TEST 6
#define INTERWORKING_ANT_WILDCARD 15


/* Advertisement Protocol ID definitions (IEEE Std 802.11u-2011) */
enum adv_proto_id {
    ACCESS_NETWORK_QUERY_PROTOCOL = 0,
    MIH_INFO_SERVICE = 1,
    MIH_CMD_AND_EVENT_DISCOVERY = 2,
    EMERGENCY_ALERT_SYSTEM = 3,
    ADV_PROTO_VENDOR_SPECIFIC = 221
};

/* Access Network Query Protocol info ID definitions (IEEE Std 802.11u-2011) */
enum anqp_info_id {
    ANQP_QUERY_LIST = 256,
    ANQP_CAPABILITY_LIST = 257,
    ANQP_VENUE_NAME = 258,
    ANQP_EMERGENCY_CALL_NUMBER = 259,
    ANQP_NETWORK_AUTH_TYPE = 260,
    ANQP_ROAMING_CONSORTIUM = 261,
    ANQP_IP_ADDR_TYPE_AVAILABILITY = 262,
    ANQP_NAI_REALM = 263,
    ANQP_3GPP_CELLULAR_NETWORK = 264,
    ANQP_AP_GEOSPATIAL_LOCATION = 265,
    ANQP_AP_CIVIC_LOCATION = 266,
    ANQP_AP_LOCATION_PUBLIC_URI = 267,
    ANQP_DOMAIN_NAME = 268,
    ANQP_EMERGENCY_ALERT_URI = 269,
    ANQP_TDLS_CAPABILITY = 270,
    ANQP_EMERGENCY_NAI = 271,
    ANQP_NEIGHBOR_REPORT = 272,
    ANQP_VENUE_URL = 277,
    ANQP_ADVICE_OF_CHARGE = 278,
    ANQP_LOCAL_CONTENT = 279,
    ANQP_VENDOR_SPECIFIC = 56797
};

/* NAI Realm list - EAP Method subfield - Authentication Parameter ID */
enum nai_realm_eap_auth_param {
    NAI_REALM_EAP_AUTH_EXPANDED_EAP_METHOD = 1,
    NAI_REALM_EAP_AUTH_NON_EAP_INNER_AUTH = 2,
    NAI_REALM_EAP_AUTH_INNER_AUTH_EAP_METHOD = 3,
    NAI_REALM_EAP_AUTH_EXPANDED_INNER_EAP_METHOD = 4,
    NAI_REALM_EAP_AUTH_CRED_TYPE = 5,
    NAI_REALM_EAP_AUTH_TUNNELED_CRED_TYPE = 6,
    NAI_REALM_EAP_AUTH_VENDOR_SPECIFIC = 221
};

enum nai_realm_eap_auth_inner_non_eap {
    NAI_REALM_INNER_NON_EAP_PAP = 1,
    NAI_REALM_INNER_NON_EAP_CHAP = 2,
    NAI_REALM_INNER_NON_EAP_MSCHAP = 3,
    NAI_REALM_INNER_NON_EAP_MSCHAPV2 = 4
};

enum nai_realm_eap_cred_type {
    NAI_REALM_CRED_TYPE_SIM = 1,
    NAI_REALM_CRED_TYPE_USIM = 2,
    NAI_REALM_CRED_TYPE_NFC_SECURE_ELEMENT = 3,
    NAI_REALM_CRED_TYPE_HARDWARE_TOKEN = 4,
    NAI_REALM_CRED_TYPE_SOFTOKEN = 5,
    NAI_REALM_CRED_TYPE_CERTIFICATE = 6,
    NAI_REALM_CRED_TYPE_USERNAME_PASSWORD = 7,
    NAI_REALM_CRED_TYPE_NONE = 8,
    NAI_REALM_CRED_TYPE_ANONYMOUS = 9,
    NAI_REALM_CRED_TYPE_VENDOR_SPECIFIC = 10
};

/*
 * IEEE P802.11-REVmc/D5.0 Table 9-81 - Measurement type definitions for
 * measurement requests
 */
enum measure_type {
    MEASURE_TYPE_BASIC = 0,
    MEASURE_TYPE_CCA = 1,
    MEASURE_TYPE_RPI_HIST = 2,
    MEASURE_TYPE_CHANNEL_LOAD = 3,
    MEASURE_TYPE_NOISE_HIST = 4,
    MEASURE_TYPE_BEACON = 5,
    MEASURE_TYPE_FRAME = 6,
    MEASURE_TYPE_STA_STATISTICS = 7,
    MEASURE_TYPE_LCI = 8,
    MEASURE_TYPE_TRANSMIT_STREAM = 9,
    MEASURE_TYPE_MULTICAST_DIAG = 10,
    MEASURE_TYPE_LOCATION_CIVIC = 11,
    MEASURE_TYPE_LOCATION_ID = 12,
    MEASURE_TYPE_DIRECTIONAL_CHAN_QUALITY = 13,
    MEASURE_TYPE_DIRECTIONAL_MEASURE = 14,
    MEASURE_TYPE_DIRECTIONAL_STATS = 15,
    MEASURE_TYPE_FTM_RANGE = 16,
    MEASURE_TYPE_MEASURE_PAUSE = 255,
};

/* IEEE Std 802.11-2012 Table 8-71 - Location subject definition */
enum location_subject {
    LOCATION_SUBJECT_LOCAL = 0,
    LOCATION_SUBJECT_REMOTE = 1,
    LOCATION_SUBJECT_3RD_PARTY = 2,
};

/*
 * IEEE P802.11-REVmc/D5.0 Table 9-94 - Optional subelement IDs for LCI request
 */
enum lci_req_subelem {
    LCI_REQ_SUBELEM_AZIMUTH_REQ = 1,
    LCI_REQ_SUBELEM_ORIGINATOR_MAC_ADDR = 2,
    LCI_REQ_SUBELEM_TARGET_MAC_ADDR = 3,
    LCI_REQ_SUBELEM_MAX_AGE = 4,
};


/*
 * IEEE 802.11 mac frame foramt
 */

typedef struct beacon_cap_t {
	u16 ess_capa:1;
	u16 ibss:1;
	u16 cfp_1:2;
	u16 privacy:1;
	u16 preamble:1;
	u16 pbcc:1;
	u16 channel_agility:1;
	u16 spec_mgnt:1;
	u16 cfp_2:1;
	u16 short_slot_time:1;
	u16 auto_power_save:1;
	u16 radio_mesurment:1;
	u16 dsss_ofdm:1;
	u16 block_ack:1;
	u16 imm_block_ack:1;
} STRUCT_PACKED beacon_cap_t; // 2 byte

typedef struct h80211_fragseq_t {
	u16 frag:4;
	u16 seq:12;
} STRUCT_PACKED h80211_fragseq_t; // 2 byte

#define GET_WLAN_FROMDS(ph80211) \
	(((h80211_hdr_t *)(ph80211))->fc.flags.fromds)
#define GET_WLAN_TODS(ph80211) \
	(((h80211_hdr_t *)(ph80211))->fc.flags.tods)
#define GET_WLAN_TYPE(ph80211) \
	(((h80211_hdr_t *)(ph80211))->fc.type)
#define GET_WLAN_SUBTYPE(ph80211) \
	(((h80211_hdr_t *)(ph80211))->fc.subtype)


typedef struct h80211_hdr_t {
	struct {
		u8 version:2;
		u8 type:2;
		u8 subtype:4;
		struct {
			u8 tods:1;
			u8 fromds:1;
			u8 more_frag:1;
			u8 retry:1;
			u8 pwr_mgt:1;
			u8 more_data:1;
			u8 protect:1;
			u8 order:1;
		} STRUCT_PACKED flags;
	} STRUCT_PACKED fc;
	le16 duration;
	struct {
		union {
			struct {
				u8 addr1[6];
				u8 addr2[6];
				u8 addr3[6];
			} STRUCT_PACKED an;
			struct {
				u8 da[6];
				u8 sa[6];
				u8 bssid[6];
			} STRUCT_PACKED ad;
		} u;
	} STRUCT_PACKED addr;
	h80211_fragseq_t fragseq;
	u8 addr4[];
} STRUCT_PACKED h80211_hdr_t;


typedef struct h80211_mgmt_t {
	struct {
		u8 version:2;
		u8 type:2;
		u8 subtype:4;
		struct {
			u8 tods:1;
			u8 fromds:1;
			u8 more_frag:1;
			u8 retry:1;
			u8 pwr_mgt:1;
			u8 more_data:1;
			u8 protect:1;
			u8 order:1;
		} STRUCT_PACKED flags;
	} STRUCT_PACKED fc;

	le16 duration;

	struct {
		union {
			struct {
				u8 addr1[6];
				u8 addr2[6];
				u8 addr3[6];
			} STRUCT_PACKED an;
			struct {
				u8 da[6];
				u8 sa[6];
				u8 bssid[6];
			} STRUCT_PACKED ad;
		} u;
	} STRUCT_PACKED addr;

	h80211_fragseq_t fragseq;

	union {
		struct {
			ull timestamp;             // 8 byte
			u16 beacon_int;            // 2 byte
			beacon_cap_t capability;   // 2 byte
			/* followed by some of SSID, Supported rates,
			 * FH Params, DS Params, CF Params, IBSS Params */
			u8 variable[];
		} STRUCT_PACKED beacon;       // total: 12 byte

		struct {
			u8 timestamp[8];
			le16 beacon_int;
			beacon_cap_t capability;   // 2 byte
			/* followed by some of SSID, Supported rates,
			 * FH Params, DS Params, CF Params, IBSS Params */
			u8 variable[];
		}STRUCT_PACKED probe_resp;

		struct {
			beacon_cap_t capability;   // 2 byte
			u16 listen_int;            // 2 byte
			/* followed by SSID and Supported rates */
			u8 variable[];
		} STRUCT_PACKED assoc_req;    // total: 4 byte

		struct {
			beacon_cap_t capability;   // 2 byte
			u16 listen_int;            // 2 byte
			u8 cur_ap[6];              // 6 byte
			/* followed by SSID and Supported rates */
			u8 variable[];
		} STRUCT_PACKED reassoc_req;  // total: 10 byte

		struct {
			beacon_cap_t capability;   // 2 byte
			u16 status;                // 2 byte, 0x0000 = successful
			u16 assoc_id;              // 2 byte
			/* followed by Supported rates */
			u8 variable[];
		} STRUCT_PACKED assoc_resp, reassoc_resp;   // total: 6 byte

		struct {
			le16 algo;                // 2 byte (0: open system, 1: shared key)
			le16 xid;                 // 2 byte
			le16 status;              // 2 byte (0x0000: successful)
			/* possibly followed by Challenge text */
			u8   variable[];
		} STRUCT_PACKED auth;        // total: 6 byte

		struct {
			le16 reason;              // 2 byte
			u8 variable[];            //
		} STRUCT_PACKED deauth;      // total: 2 byte

		struct {
			le16 reason;              // 2 byte
			u8 variable[];            //
		} STRUCT_PACKED disassoc;    // total: 2 byte

		struct {
			u8 dsap;                  // 1 byte
			u8 ssap;                  // 1 byte
			u8 ctrl;                  // 1 byte
			u8 oui[3];                // 3 byte
			u8 pid[2];                // 2 byte
		} STRUCT_PACKED llc;         // total: 8 byte

		struct {
			u8 addr4[6];
			u8 data[];
		} STRUCT_PACKED wds;

		struct {
			u8 category;
			union {
				struct {
					u8 action_code;
					u8 dialog_token;
					u8 status_code;
					u8 variable[];
				} STRUCT_PACKED wmm_action;
				struct {
					u8 action_code;
					u8 element_id;
					u8 length;
					u8 switch_mode;
					u8 new_chan;
					u8 switch_count;
				} STRUCT_PACKED chan_switch;
				struct {
					u8 action;
					u8 sta_addr[ETH_ALEN];
					u8 target_ap_addr[ETH_ALEN];
					u8 variable[]; /* FT Request */
				} STRUCT_PACKED ft_action_req;
				struct {
					u8 action;
					u8 sta_addr[ETH_ALEN];
					u8 target_ap_addr[ETH_ALEN];
					le16 status_code;
					u8 variable[]; /* FT Request */
				} STRUCT_PACKED ft_action_resp;
				struct {
					u8 action;
					u8 trans_id[WLAN_SA_QUERY_TR_ID_LEN];
				} STRUCT_PACKED sa_query_req;
				struct {
					u8 action; /* */
					u8 trans_id[WLAN_SA_QUERY_TR_ID_LEN];
				} STRUCT_PACKED sa_query_resp;
				struct {
					u8 action;
					u8 dialogtoken;
					u8 variable[];
				} STRUCT_PACKED wnm_sleep_req;
				struct {
					u8 action;
					u8 dialogtoken;
					le16 keydata_len;
					u8 variable[];
				} STRUCT_PACKED wnm_sleep_resp;
				struct {
					u8 action;
					u8 variable[];
				} STRUCT_PACKED public_action;
				struct {
					u8 action; /* 9 */
					u8 oui[3];
					/* Vendor-specific content */
					u8 variable[];
				} STRUCT_PACKED vs_public_action;
				struct {
					u8 action; /* 7 */
					u8 dialog_token;
					u8 req_mode;
					le16 disassoc_timer;
					u8 validity_interval;
					/* BSS Termination Duration (optional),
					 * Session Information URL (optional),
					 * BSS Transition Candidate List
					 * Entries */
					u8 variable[];
				} STRUCT_PACKED bss_tm_req;
				struct {
					u8 action; /* 8 */
					u8 dialog_token;
					u8 status_code;
					u8 bss_termination_delay;
					/* Target BSSID (optional),
					 * BSS Transition Candidate List
					 * Entries (optional) */
					u8 variable[];
				} STRUCT_PACKED bss_tm_resp;
				struct {
					u8 action; /* 6 */
					u8 dialog_token;
					u8 query_reason;
					/* BSS Transition Candidate List
					 * Entries (optional) */
					u8 variable[];
				} STRUCT_PACKED bss_tm_query;
				struct {
					u8 action; /* 15 */
					u8 variable[];
				} STRUCT_PACKED slf_prot_action;
				struct {
					u8 action;
					u8 variable[];
				} STRUCT_PACKED fst_action;
				struct {
					u8 action;
					u8 dialog_token;
					u8 variable[];
				} STRUCT_PACKED rrm;
			} u;
		} STRUCT_PACKED action;
	} u;
} STRUCT_PACKED h80211_mgmt_t;


#endif
