/*
 * convert.h
 *
 *  Created on: 2017. 8. 1.
 *      Author: root
 */

#ifndef SRC_CONVERT_H_
#define SRC_CONVERT_H_

#include <stdint.h>
#include <sys/time.h>

char *new_macstr(unsigned char *mac);
int hexchar2int(unsigned char c);
int hex2int(char *s, int len);
long usec2msec(struct timeval *tv);
char *new_ltoa(unsigned long num, int base);
char *new_itoa(int num, int base);
char *new_utoa(unsigned int num, int base);
char *new_u64toa(uint64_t num, int base);


#endif /* SRC_CONVERT_H_ */
