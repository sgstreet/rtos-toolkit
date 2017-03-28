/*
 * Copyright (C) 2017 Red Rocket Computing, LLC
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * brk-allocator.c
 *
 * Created on: Mar 28, 2017
 *     Author: Stephen Street (stephen@redrocketcomputing.com)
 */

#include <assert.h>
#include <errno.h>

#include <memory/brk-alloc.h>

#define ALIGN(v, a) ((void *)(((unsigned long)v + (a - 1)) & ~(a - 1)))
#define ROUND(v, a) (((unsigned long)v + (a - 1)) & ~(a - 1))

int brk_alloc_init(struct brk_allocator *allocator, void *start, void *end)
{
	assert(allocator != 0);

	/* The brk allocator is a simple incrementing memory manager, make sure there is a valid memory range */
	if (end < start) {
		errno = EINVAL;
		return -1;
	}

	/* Save the memory setup */
	allocator->start = ALIGN(start, 8);
	allocator->end = end;

	/* All good */
	return 0;
}

struct brk_allocator *brk_alloc_create(void *start, void *end)
{
	/* Place metadata inside the allocation area */
	struct brk_allocator *allocator = ALIGN(start, 8);

	/* Forward to init */
	if (brk_alloc_init(allocator, start + sizeof(struct brk_allocator), end))
		return 0;

	/* All good */
	return allocator;
}

int brk_alloc_destroy(struct brk_allocator *allocator)
{
	assert(allocator != 0);

	/* Force all future allocations to fail */
	allocator->start = allocator->end;

	/* All done */
	return 0;
}

void *brk_alloc(struct brk_allocator *allocator, size_t size)
{
	assert(allocator != 0);

	/* Round size to multiple of double words */
	size = ROUND(size, 8);

	/* Do we have enough? */
	if (allocator->start + size > allocator->end) {
		errno = ENOMEM;
		return 0;
	}

	/* Yes, update the start */
	void *ptr = allocator->start;
	allocator->start += size;

	/* Return the memory */
	return ptr;
}

void brk_release(struct brk_allocator *allocator, void *ptr, size_t size)
{
	assert(allocator != 0);

	/* Never fail on null pointers */
	if (ptr == 0)
		return;

	/* Maybe abort execution, depending on configuration */
	brk_abort();
}

void brk_free(struct brk_allocator *allocator, void *ptr)
{
	assert(allocator != 0);

	/* Never fail on null pointers */
	if (ptr == 0)
		return;

	/* Maybe abort execution, depending on configuration */
	brk_abort();
}



