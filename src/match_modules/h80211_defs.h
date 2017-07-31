#ifndef _H80211_DEFS_
#define _H80211_DEFS_

#include <stdint.h>

#define MAX_IE_ELEMENT_SIZE 256

typedef struct WI_flags_t {
    uint8_t   tods:1;
    uint8_t   fromds:1;
    uint8_t   more_frag:1; 
    uint8_t   retry:1;
    uint8_t   pwr_mgt:1;
    uint8_t   more_data:1;
    uint8_t   protect:1;
    uint8_t   order_flag:1;
} __packed 
WI_flags_t; // 1 byte

typedef struct WI_fc_t {
    uint8_t    version:2;
    uint8_t    type:2;
    uint8_t    subtype:4;
    WI_flags_t flags; // 1 byte
} __packed
WI_fc_t; // 2 byte

typedef struct WI_fragseq_t {
    uint16_t    frag_number:4;
    uint16_t    seq_number:12;
} __packed
WI_fragseq_t; // 2 byte

typedef struct WI_hdr_t {
    WI_fc_t      fc;        // 2 byte
    uint16_t     duration;  // 2 byte
    uint8_t      addr1[6];  // 6 byte
    uint8_t      addr2[6];  // 6 byte
    uint8_t      addr3[6];  // 6 byte 
    WI_fragseq_t frag_seq;  // 2 byte
    uint8_t      addr4[0];  // 0 byte (6 byte)
} __packed
WI_hdr_t; // 24 byte (30 byte)

typedef struct WI_ctrl_hdr_t {
    WI_fc_t  fc;       // 2 byte
    uint16_t duration; // 2 byte
    uint8_t  addr1[6]; // 6 byte
    uint8_t  addr2[0]; // 0 byte (6 byte)
} __packed
WI_ctrl_hdr_t; // 10 byte (16 byte)

typedef struct WI_tag_t {
    uint8_t   tag;
    uint8_t   len;
    uint8_t   data[0];
} __packed 
WI_tag_t;

typedef struct WI_beacon_cap_t {
    uint16_t   ess_capa:1;
    uint16_t   ibss:1;
    uint16_t   cfp_1:2;
    uint16_t   privacy:1;
    uint16_t   preamble:1;
    uint16_t   pbcc:1;
    uint16_t   channel_agility:1;
    uint16_t   spec_mgnt:1;
    uint16_t   cfp_2:1;
    uint16_t   short_slot_time:1;
    uint16_t   auto_power_save:1;
    uint16_t   radio_mesurment:1;
    uint16_t   dsss_ofdm:1;
    uint16_t   block_ack:1;
    uint16_t   imm_block_ack:1;
} __packed
WI_beacon_cap_t; // 2 byte

typedef struct WI_beacon_t {
    unsigned long long timestamp; // 8 byte
    uint16_t           intv;      // 2 byte (beacon interval)
    WI_beacon_cap_t    cap;      // 2 byte
} __packed
WI_beacon_t; // 12 byte

typedef struct WI_assoc_req_t {
    WI_beacon_cap_t cap;        // 2 byte
    uint16_t        listen_intv; // 2 byte
} __packed
WI_assoc_req_t;   // 4 byte

typedef struct WI_reassoc_req_t {
    WI_beacon_cap_t cap;        // 2 byte
    uint16_t        listen_intv; // 2 byte
    uint8_t         cur_ap[6];   // 6 byte
} __packed
WI_reassoc_req_t; // 10 byte

typedef struct WI_assoc_resp_t {
    WI_beacon_cap_t cap;       // 2 byte
    uint16_t        status;     // 2 byte, 0x0000 = successful
    uint16_t        assoc_id;   // 2 byte
} __packed
WI_assoc_resp_t; // 6 byte

typedef WI_assoc_resp_t
WI_reassoc_resp_t; // 6 byte

typedef struct WI_auth_t {
    uint16_t   algorithm; // 2 byte (0: open system, 1: shared key)
    uint16_t   seq;       // 2 byte
    uint16_t   status;    // 2 byte (0x0000: successful)
} __packed
WI_auth_t; // 6 byte

typedef struct WI_deauth_t {
    uint16_t   reason; // 2 byte
} __packed
WI_deauth_t; // 2 byte

typedef struct WI_ba_ctrl_t {
    uint16_t   immediate_ack:1;
    uint16_t   multi_tid:1;
    uint16_t   compress_bitmap:1;
    uint16_t   reserved:9;
    uint16_t   tid_for_frame:4;
} __packed
WI_ba_ctrl_t; // 2 byte

typedef struct WI_ba_startseq_ctrl_t {
    uint16_t   fragment:4;
    uint16_t   starting_seq:12;
} __packed
WI_ba_startseq_ctrl_t; // 2 byte

typedef struct WI_ba_bitmap_t {
    uint8_t    bitmap[8];
} __packed
WI_ba_bitmap_t; // 8 byte

typedef struct WI_qos_ctrl_t {
    uint16_t   priority:3;
    uint16_t   not_use:1; // not use
    uint16_t   eosp:1;
    uint16_t   ack_policy:2;
    uint16_t   payload_type:1;
    uint16_t   qap_ps_buffer_state:8;
} __packed
WI_qos_ctrl_t; // 2 byte

typedef struct WI_llc_t {
    uint8_t dsap;
    uint8_t ssap;
    uint8_t ctrl;
    uint8_t oui[3];
    uint8_t pid[2];
} __packed
WI_llc_t; // 8 byte

#endif
