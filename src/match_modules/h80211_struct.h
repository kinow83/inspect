#ifndef _H80211_STRUCT_
#define _H80211_STRUCT_

#include "types.h"

struct h80211_flags {
	u8 tods:1;
	u8 fromds:1;
	u8 more_frag:1;
	u8 retry:1;
	u8 pwr_mgt:1;
	u8 more_data:1;
	u8 protect:1;
	u8 order_flag:1;
} STRUCT_PACKED; // 1 byte

struct h80211_fc {
	u8 ver:2;
	u8 type:2
	u8 subtype:4;
	struct h80211_flags flags; // 1 byte
} STRUCT_PACKED; // 1 byte

struct h80211_fragseq {
	u16 frag:4;
	u16 seq:12;
} STRUCT_PACKED; // 2 byte

struct h80211_hdr {
	struct h80211_fc fc;
	le16 duration;
	u8 addr1[6];
	u8 addr2[6];
	u8 addr3[6];
	struct h80211_fragseq fragseq;
	u8 addr4[0];
} STRUCT_PACKED;

struct h80211_mgmt {
	le16 fc;
	le16 duration;
	u8 da[6];
	u8 sa[6];
	u8 bssid[6];
	le16 seq;
	union {
		struct {
			le16 auth_alg;
			le16 auth_xid;
			le16 auth_status;
			/* possibly followed by Challenge text */
			u8   variable[];
		} STRUCT_PACKED auth;
		struct {
			le16 deauth_reason;
			u8   variable[];
		} STRUCT_PACKED deauth; 
		struct {
			
		} STRUCT_PACKED assoc_req;
	};
};


#endif
