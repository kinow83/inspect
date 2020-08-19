#ifndef _FORMAT_H_
#define _FORMAT_H_

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "typedef.h"

// static snprintf
#define SNP(b, ...) ({ snprintf((b), sizeof(b), __VA_ARGS__); })

#define MAC_NULL (u8 *)"\x00\x00\x00\x00\x00\x00"
#define MAC_BCAST (u8 *)"\xFF\xFF\xFF\xFF\xFF\xFF"

typedef u8 mac_t[6];
typedef u8 oui_t[3];

#define mac_cmp(m1, m2)    (memcmp((void *)m1, (void *)m2, 6) == 0)
#define mac_cmp_bcast(m1)  (memcmp((void *)m1, (void *)MAC_BCAST, 6) == 0)
#define mac_cmp_null(m1)   (memcmp((void *)m1, (void *)MAC_NULL, 6) == 0)

#define mac_copy(to, from) (memcpy((void *)(to), (void *)(from), 6))
#define mac_copy_bcast(to) (memcpy((void *)(to), (void *)(MAC_BCAST), 6))
#define mac_copy_null(to)  (memcpy((void *)(to), (void *)(MAC_NULL), 6))

#define oui_cmp(o1, o2)    (memcmp((void*)(o1), (void*)(o2), 3) == 0)
#define oui_cpy(to, from)  (memcpy((void*)(to), (void*)(from), 3))


/*
 * mac address format
 */
#define _MAC_FMT_ "%02x:%02x:%02x:%02x:%02x:%02x"
#define _MAC_FMT_FILL_(x) \
     ((u8 *)(x))[0], \
	 ((u8 *)(x))[1], \
	 ((u8 *)(x))[2], \
	 ((u8 *)(x))[3], \
	 ((u8 *)(x))[4], \
	 ((u8 *)(x))[5]
#define MACUINT64(x) \
	(((u64)x[0] << (5 * 8)) | \
	 ((u64)x[1] << (4 * 8)) | \
	 ((u64)x[2] << (3 * 8)) | \
	 ((u64)x[3] << (2 * 8)) | \
	 ((u64)x[4] << (1 * 8)) | \
	 ((u64)x[5] << (0 * 8)))

#define _IP_FMT_ "%d.%d.%d.%d"
#if __BYTE_ORDER == __BIG_ENDIAN
	#define _IP_FMT_FILL_(x) \
		((u8 *)(x))[0], ((u8 *)(x))[1], ((u8 *)(x))[2], ((u8 *)(x))[3]
	#define IS_MCAST_IP(x) \
		((((u8*)(x))[0] >> 4) == 0x0E)
#else
	#define _IP_FMT_FILL_(x) \
		((u8 *)(x))[3], ((u8 *)(x))[2], ((u8 *)(x))[1], ((u8 *)(x))[0]
	#define IS_MCAST_IP(x) \
		((((u8 *)(x))[3] >> 4) == 0x0E)
#endif



#endif
