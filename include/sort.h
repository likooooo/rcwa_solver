#pragma once
#include <stddef.h>
void
sort (void *const pbase, size_t total_elems, size_t size,
	int(*cmp)(const void *, const void *, void *arg),
	void *arg);
