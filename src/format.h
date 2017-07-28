#ifndef _FORMAT_H_
#define _FORMAT_H_

#include <stdio.h>
#include <string.h>
#include <stdint.h>

// static snprintf
#define SNP(b, ...) ({ snprintf((b), sizeof(b), __VA_ARGS__); })

#define MAC_NULL (uint8_t*)"\x00\x00\x00\x00\x00\x00"
#define MAC_BCAST (uint8_t*)"\xFF\xFF\xFF\xFF\xFF\xFF"

typedef uint8_t mac_t[6];
typedef uint8_t oui_t[3];

#define mac_cmp(m1, m2)   (memcmp((void*)m1, (void*)m2, 6) == 0)
#define mac_cmp_bcast(m1) (memcmp((void*)m1, (void*)MAC_BCAST, 6) == 0)
#define mac_cmp_null(m1)  (memcmp((void*)m1, (void*)MAC_NULL, 6) == 0)

#define mac_copy(m1, m2)   (memcpy((void*)(m1), (void*)(m2), 6))
#define mac_copy_bcast(m1) (memcpy((void*)(m1), (void*)(MAC_BCAST), 6))
#define mac_copy_null(m1)  (memcpy((void*)(m1), (void*)(MAC_NULL), 6))

/*
 * mac address format
 */
#define _MAC_FMT_ "%02x:%02x:%02x:%02x:%02x:%02x"
#define _MAC_FMT_FILL_(x) \
     ((uint8_t*)(x))[0], \
	 ((uint8_t*)(x))[1], \
	 ((uint8_t*)(x))[2], \
	 ((uint8_t*)(x))[3], \
	 ((uint8_t*)(x))[4], \
	 ((uint8_t*)(x))[5]
#define MACUINT64(x) \
	(((uint64_t)x[0] << (5 * 8)) | \
	 ((uint64_t)x[1] << (4 * 8)) | \
	 ((uint64_t)x[2] << (3 * 8)) | \
	 ((uint64_t)x[3] << (2 * 8)) | \
	 ((uint64_t)x[4] << (1 * 8)) | \
	 ((uint64_t)x[5] << (0 * 8)))

#define _IP_FMT_ "%d.%d.%d.%d"
#if __BYTE_ORDER == __BIG_ENDIAN
	#define _IP_FMT_FILL_(x) \
		((uint8_t*)(x))[0], ((uint8_t*)(x))[1], \
		((uint8_t*)(x))[2], ((uint8_t*)(x))[3]
	#define IS_MCAST_IP(x) \
		((((uint8_t*)(x))[0] >> 4) == 0x0E)
#else
	#define _IP_FMT_FILL_(x) \
		((uint8_t*)(x))[3], ((uint8_t*)(x))[2], \
		((uint8_t*)(x))[1], ((uint8_t*)(x))[0]
	#define IS_MCAST_IP(x) \
		((((uint8_t*)(x))[3] >> 4) == 0x0E)
#endif



#endif
