/*
 * Copyright (C) 2017 Red Rocket Computing, LLC
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * monotonic-alloc.c
 *
 * Created on: Apr 24, 2017
 *     Author: Stephen Street (stephen@redrocketcomputing.com)
 */

#include <errno.h>
#include <stdint.h>

#include <util/container_of.h>
#include <util/util.h>
#include <memory/monotonic-alloc.h>

static void monotonic_destroy(struct mem_allocator *allocator)
{
	struct monotonic_allocator *monotonic = container_of_or_null(allocator, struct monotonic_allocator, parent);

	assert(monotonic != 0);

	/* Release memory if dynamically allocated */
	if (monotonic->allocated)
		mem_release(allocator->upstream, monotonic, monotonic->size + sizeof(struct monotonic_allocator), 8);
}

static void *monotonic_allocate(struct mem_allocator *allocator, size_t size, size_t alignment)
{
	struct monotonic_allocator *monotonic = container_of_or_null(allocator, struct monotonic_allocator, parent);

	assert(monotonic != 0);

	/* Minimum allocation is this minimum alignment */
	size = max(size + (alignment - 1), MEM_MIN_ALIGN);

	/* Do we have enough? */
	if (monotonic->start + size > monotonic->start + monotonic->size) {
		errno = ENOMEM;
		return 0;
	}

	/* Yes, update the start */
	void *ptr = (void *)((((uintptr_t)(monotonic->start)) + size) & ~(alignment - 1));
	monotonic->start += size;

	/* Return the memory */
	return ptr;
}

static void monotonic_release(struct mem_allocator *allocator, void *ptr, size_t size, size_t alignment)
{
	assert(allocator != 0);

	/* Do nothing */
}

static size_t monotonic_used(const struct mem_allocator *allocator)
{
	const struct monotonic_allocator *monotonic = container_of_or_null(allocator, struct monotonic_allocator, parent);

	assert(monotonic != 0);

	/* Return the amount between current and start */
	return monotonic->current - monotonic->start;
}

static size_t monotonic_available(const struct mem_allocator *allocator)
{
	const struct monotonic_allocator *monotonic = container_of_or_null(allocator, struct monotonic_allocator, parent);

	assert(monotonic != 0);

	/* Return the amount between end and current */
	return monotonic->start + monotonic->size - monotonic->current;
}


const struct mem_allocator_ops monotonic_allocator_ops =
{
	.allocate = monotonic_allocate,
	.release = monotonic_release,
	.destroy = monotonic_destroy,
	.used = monotonic_used,
	.available = monotonic_available,
};

int monotonic_allocator_init(struct monotonic_allocator *allocator, struct mem_allocator *upstream, void *memory, size_t size)
{
	/* Forward to infrastructure */
	int status = mem_allocator_init(&allocator->parent, upstream, &monotonic_allocator_ops, MEM_ALLOC_CAP_ALIGN | MEM_ALLOC_CAP_STATS);
	if (status < 0)
		return status;

	/* Initialize */
	allocator->start = memory;
	allocator->size = size;
	allocator->current = MEM_ALLOC_ALIGN(memory, 8);
	allocator->allocated = false;

	/* All good */
	return 0;
}

struct mem_allocator *monotonic_allocator_create(struct mem_allocator *upstream, void *memory, size_t size)
{
	/* Meta data is stored in the memory region, which at this point maybe null */
	struct monotonic_allocator *allocator = memory;

	/* Do we need to allocate a memory block form the allocator */
	if (memory == 0) {

		/* Can we really do this */
		if (upstream == 0) {
			errno = ENOMEM;
			return 0;
		}

		/* Get the memory block */
		allocator = mem_allocate(upstream, size + sizeof(struct monotonic_allocator), 8);
		if (!allocator) {
			errno = ENOMEM;
			return 0;
		}
	}

	/* Initialize in place */
	int status = monotonic_allocator_init(allocator, upstream, (void *)allocator + sizeof(struct monotonic_allocator), size);
	if (status < 0) {

		/* Initialize failed, release memory block if we allocated it */
		if (memory == 0)
			mem_release(upstream, allocator, size + sizeof(struct monotonic_allocator), 8);

		/* errno already initialized, just indicate error */
		return 0;
	}

	/* Mark as dynamically allocated */
	allocator->allocated = memory == 0;

	/* All done */
	return &allocator->parent;
}
