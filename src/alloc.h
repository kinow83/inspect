#ifndef _ALLOCATE_
#define _ALLOCATE_

#include <string.h>

#define alloc1(type) \
	(type *)_calloc_1_(sizeof(type)) 

void *_calloc_1_(size_t size);

#endif
