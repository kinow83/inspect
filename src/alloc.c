#include "log.h"
#include "alloc.h"

inline void *_calloc_sizeof_(size_t size)
{
	void *_new = calloc(1, size);
	if (!_new) {
		echo.f("fail alloc new memrory");
	}
	return _new;
}

