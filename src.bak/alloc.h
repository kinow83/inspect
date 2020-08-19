#ifndef _ALLOCATE_
#define _ALLOCATE_

#include <string.h>

#define alloc_sizeof(type) \
	(type *)_calloc_sizeof_(sizeof(type))

#define alloc_type(type, size) \
	(type *)_calloc_sizeof_(size)

void *_calloc_sizeof_(size_t size);

#endif
