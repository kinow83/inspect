/*
 * convert.c
 *
 *  Created on: 2017. 8. 1.
 *      Author: root
 */

#include <stdio.h>
#include <string.h>


// Converts a mac address in a human-readable format
char *new_macstr(unsigned char *mac)
{
    char *str =  (char *)malloc(sizeof(char)*18);
    snprintf(str, 18, "%02x:%02x:%02x:%02x:%02x:%02x",
    		*mac, *(mac+1), *(mac+2), *(mac+3), *(mac+4), *(mac+5));
    return str;
}

/* Return -1 if it's not an hex value and return its value when it's a hex value */
int hexchar2int(unsigned char c)
{
	static int table_created = 0;
	static int table[256];

	int i;

	if (table_created == 0) {
		/*
		 * It may seem a bit long to calculate the table
		 * but character position depend on the charset used
		 * Example: EBCDIC
		 * but it's only done once and then conversion will be really fast
		 */
		for (i = 0; i < 256; i++) {
			switch ((unsigned char) i) {
			case '0':
				table[i] = 0;
				break;
			case '1':
				table[i] = 1;
				break;
			case '2':
				table[i] = 2;
				break;
			case '3':
				table[i] = 3;
				break;
			case '4':
				table[i] = 4;
				break;
			case '5':
				table[i] = 5;
				break;
			case '6':
				table[i] = 6;
				break;
			case '7':
				table[i] = 7;
				break;
			case '8':
				table[i] = 8;
				break;
			case '9':
				table[i] = 9;
				break;
			case 'A':
			case 'a':
				table[i] = 10;
				break;
			case 'B':
			case 'b':
				table[i] = 11;
				break;
			case 'C':
			case 'c':
				table[i] = 12;
				break;
			case 'D':
			case 'd':
				table[i] = 13;
				break;
			case 'E':
			case 'e':
				table[i] = 14;
				break;
			case 'F':
			case 'f':
				table[i] = 15;
				break;
			default:
				table[i] = -1;
			}
		}
		table_created = 1;
	}
	return table[c];
}

int hex2int(char *s, int len)
{
	int i = 0;
	int convert = -1;
	int value = 0;

	// Remove leading 0 (and also the second char that can be x or X)
	while (i < len) {
		if (s[i] != '0' || (i == 1 && toupper((int) s[i]) != 'X')) {
			break;
		}
		++i;
	}

	// Convert to hex
	while (i < len) {
		convert = hexchar2int((unsigned char) s[i]);
		// If conversion failed, return -1
		if (convert == -1) {
			return -1;
		}
		value = (value * 16) + convert;
		++i;
	}
	return value;
}

long usec2msec(struct timeval *tv)
{
    long msec;
    msec = (tv->tv_sec * 1000) + (tv->tv_usec / 1000);
    return msec;
}

char *new_ltoa(unsigned long num, int base)
{
	unsigned long sum;
	unsigned long digit;
	int len = 0, tot = 0;
	char *buf;

	sum = num;
	do {
		digit = sum % base;
		len++;
		sum /= base;
	} while (sum);

	buf = (char *) malloc(len + 2);
	tot = len;
	sum = num;
	len--;
	do {
		digit = sum % base;
		if (digit < 0xA)
			buf[len--] = '0' + digit;
		else
			buf[len--] = 'A' + digit - 0xA;
		sum /= base;
	} while (sum);

	buf[tot] = '\0';
	return buf;
}

char *new_itoa(int num, int base)
{
    int sum;
    int digit;
    int len = 0, tot = 0;
    char *buf;
    int minus;

    minus = (num < 0) ? 1 : 0;

    // positive number
    sum = (num < 0) ? num*-1 : num;
    if (minus) len++;
    do {
        digit = sum % base;
        len++;
        sum /= base;
    } while (sum);

    buf = (char *)malloc(len+2); // include '\0'
    tot = len;
    sum = (num < 0) ? num*-1 : num;

    buf[tot--] = '\0';

    do {
        digit = sum % base;
        if (digit < 0xA)
            buf[tot--] = '0' + digit;
        else
            buf[tot--] = 'A' + digit - 0xA;
        sum /= base;
    } while (sum);

    if (minus) buf[tot] = '-';
    return buf;
}

char *new_utoa(unsigned int num, int base)
{
    unsigned int sum;
    unsigned int digit;
    int len = 0, tot = 0;
    char *buf;

    sum = num;
    do {
        digit = sum % base;
        len++;
        sum /= base;
    } while (sum);

    buf = (char *)malloc(len+2);
    tot = len;
    sum = num;
    len--;
    do {
        digit = sum % base;
        if (digit < 0xA)
            buf[len--] = '0' + digit;
        else
            buf[len--] = 'A' + digit - 0xA;
        sum /= base;
    } while (sum);

    buf[tot] = '\0';
    return buf;
}

char *new_u64toa(uint64_t num, int base)
{
    uint64_t sum;
    uint64_t digit;
    int len = 0, tot = 0;
    char *buf;

    sum = num;
    do {
        digit = sum % base;
        len++;
        sum /= base;
    } while (sum);

    buf = (char *)malloc(len+2);
    tot = len;
    sum = num;
    len--;
    do {
        digit = sum % base;
        if (digit < 0xA)
            buf[len--] = '0' + digit;
        else
            buf[len--] = 'A' + digit - 0xA;
        sum /= base;
    } while (sum);

    buf[tot] = '\0';
    return buf;
}

