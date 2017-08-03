/*
 * resource.h
 *
 *  Created on: 2017. 8. 2.
 *      Author: root
 */

#ifndef SRC_RESOURCE_H_
#define SRC_RESOURCE_H_

#include <stdint.h>
#include "types.h"
#include "format.h"
#include "osdep/osdep.h"


typedef struct Module_option_t {
	char *name;
	char *options;
	struct Module_option_t *next;
} Module_option_t;

typedef struct Module_option_list_t {
	Module_option_t *output;
	Module_option_t *parser;
	Module_option_t *match;
	Module_option_t *rtx;
} Module_option_list_t;

void free_module_option(Module_option_t *);
Module_option_t* new_module_option(const char *args);
size_t num_module_option(Module_option_t *mopt);

/*
 * cv_def: Config Value Define.
 */
#define cv_def(name, type) \
    type name; \
	u8 name##_en;
/*
 * cv_arr_def: Config Value Array Define.
 */
#define cv_arr_def(name, type, size) \
    type name[size]; \
	u8 name##_en;

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
	u8 tag;
	u8 len;
	u8 *data;
	struct Tag_t *next;
} Tag_t;

/*
 * shooter or capture config.
 * support multi action.
 */
typedef struct Config_t Config_t;
typedef struct Action_t {
	Config_t *config;
	u32 no;				// sequence number
	u8 enable;
	char *name;
	u8 channel;
	u32 dwell;
	cv_def(version, u8);   // 802.11 version
	cv_def(type, u8);      // frame type
	cv_def(subtype, u8);   // frame subtype
	cv_def(tods, u8);      // tods
	cv_def(fromds, u8);    // fromds
	cv_def(addr_count, u8);// mac address count
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
	u32 version;  // config version
	char *name;
	u32 extra_dwell;
	struct wif *capture_wif;
	struct wif *shooter_wif;
	struct Action_t *action;
} Config_t;

void free_tags(Tag_t *tag);
void free_actions(Action_t *action);
void free_config(Config_t *config);
void debug_action(Action_t *action);
void debug_config(Config_t *config);
Action_t *get_max_dwell(Config_t *config);
void sort_actions(Action_t **action);

#endif /* SRC_RESOURCE_H_ */
