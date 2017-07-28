
#ifndef _PARSER_
#define _PARSER_

#define CUR_VERSION 0

/*
 * cv_def: Config Value Define.
 */
#define cv_def(name, type) \
    type name; \
	uint8_t name##_en;
/*
 * cv_arr_def: Config Value Array Define.
 */
#define cv_arr_def(name, type, size) \
    type name[size]; \
	uint8_t name##_en;

#define cv_enabled(name) (name##_en == 1)

#define cv_enable(name) do { name##_en = 1; } while (0)

#define cv_disable(name) do { name##_en = 0; } while (0)

#define ezxml2int(xml, name, val) ({ \
		ezxml_t e = ezxml_child((xml), (name)); \
		if (e && e->txt) { \
			(val) = atoi(e->txt); \
		} \
	})

#define ezxml2chararr(xml, name, val) ({ \
		ezxml_t e = ezxml_child((xml), (name)); \
		if (e && e->txt) { \
			SNP((val), "%s", e->txt); \
		} \
	})


#define ezxml2cv(xml, name, cv) \
	do { \
		ezxml_t e = ezxml_child((xml), (name)); \
		if (e && e->txt) { \
			(cv) = atoi(e->ext); \
			cv_enable((cv)); \
		} \
	} while (0)


/*
 * 802.11 Information Element.
 */
struct tag_t {
	uint8_t tag;
	uint8_t len;
	uint8_t *data;
	struct tag_t *next;
}

/*
 * shooter or capture config.
 * support multi action.
 */
struct action_t {
	uint32_t no;				// sequence number

	uint8_t enable;
	char name[128];
	uint8_t channel;
	uint32_t dwell;

	cv_def(type, uint8_t);      // frame type
	cv_def(subtype, uint8_t);   // frame subtype
	cv_def(tods, uint8_t);      // tods
	cv_def(fromds, uint8_t);    // fromds
	cv_def(addr_count, uint8_t);// mac address count
	cv_def(addr1, mac_t);       // mac address1
	cv_def(addr2, mac_t);       // mac address2
	cv_def(addr3, mac_t);       // mac address3
	cv_def(addr4, mac_t);       // mac address4
	cv_def(ap_addr, mac_t);     // AP mac address (aka. BSSID)
	cv_def(st_addr, mac_t);		// ST mac address (station)
	cv_def(any_addr, mac_t);	// ANY mac address (any address capture)

	struct tag_t *tags;

	struct action_t *next;
}

struct config_t  {
	uint32_t version;
	struct action_t action;
}

struct config_operation {
	struct config_t* (*config_load)(const char* filename);
	void (*config_free)(struct config_t *);
	void (*config_debug)(struct config_t *);
};


#endif
