#ifndef _PARSER_
#define _PARSER_

#include <stdint.h>

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

#define cv_enabled(name) \
	({ name##_en == 1; })

#define cv_enable(name) \
	do { name##_en = 1; } while (0)

#define cv_disable(name) \
	do { name##_en = 0; } while (0)

#define cv_disable(name) \
	do { name##_en = 0; } while (0)

/*
 * 802.11 Information Element.
 */
typedef struct Tag_t {
	uint8_t tag;
	uint8_t len;
	uint8_t *data;
	struct Tag_t *next;
} Tag_t;

// predefine.
typedef struct Config_t Config_t;

/*
 * shooter or capture config.
 * support multi action.
 */
typedef struct Action_t {
	Config_t *config;
	uint32_t no;				// sequence number

	uint8_t enable;
	char *action_name;
	uint8_t channel;
	uint32_t dwell;

	cv_def(version, uint8_t);   // 802.11 version
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

	struct Tag_t *tags;

	struct Action_t *next;
} Action_t;

typedef struct Config_t  {
	uint32_t version;  // config version
	struct Action_t *action;
} Config_t;

typedef Config_t* (*Load_func_t)(const char *, void *);

typedef struct Config_module_t {
	char *config_name;                    // config module name.
	Load_func_t load;                     // load config from file.
	void *context;
	struct Config_module_t *next;
} Config_module_t;


void free_tags(Tag_t *tag);
void free_actions(Action_t *action);
void free_config(Config_t *config);
void free_config_modules(Config_module_t *mod);

Action_t *max_dwell_action(Config_t *config);
Config_t *load_config(const char *config_name, const char *args);
void register_config_module(const char *, Load_func_t, void *);
void init_config_modules();

void debug_action(Action_t *action);
void debug_config(Config_t *config);

#endif
