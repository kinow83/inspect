#ifndef _INSPECT__TYPES_
#define _INSPECT__TYPES_

#include <linux/types.h>
typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;
typedef int64_t s64;
typedef int32_t s32;
typedef int16_t s16;
typedef int8_t s8;

typedef u16 __bitwise be16;
typedef u16 __bitwise le16;
typedef u32 __bitwise be32;
typedef u32 __bitwise le32;
typedef u64 __bitwise be64;
typedef u64 __bitwise le64;

#ifdef __GNUC__
	#define STRUCT_PACKED __attribute__ ((packed))
#else
	#define STRUCT_PACKED
#endif

#endif
