/*
 * iwconfig.h
 *
 *  Created on: 2017. 8. 9.
 *      Author: root
 */

#ifndef SRC_CMDUTIL_IW_H_
#define SRC_CMDUTIL_IW_H_

#include <stdbool.h>


struct iw_info_t {
	char dev[32];
	struct iw_info_t *next;
};

void free_iw_info(struct iw_info_t *list);
size_t num_iw_info(struct iw_info_t *list);

struct iw_info_t *get_wifi_devs();
struct iw_info_t *get_wifi_monitor_devs(const char *iw_prefix);

bool airmon_ng_start(const char *dev);
bool airmon_ng_stop(const char *dev);
bool airmon_ng_multi_start(int count, ...);
bool airmon_ng_multi_stop(int count, ...);
bool airmon_ng_wi_start(struct iw_info_t *list);
bool airmon_ng_wi_stop(struct iw_info_t *list);


#endif /* SRC_CMDUTIL_IW_H_ */
