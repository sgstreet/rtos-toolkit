/*
 * Copyright (C) 2017 Red Rocket Computing, LLC
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * mem-alloc.h
 *
 * Created on: Apr 22, 2017
 *     Author: Stephen Street (stephen@redrocketcomputing.com)
 */

#ifndef MEMORY_MEM_ALLOC_H_
#define MEMORY_MEM_ALLOC_H_

#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdalign.h>
#include <stdint.h>

#define MEM_ALLOC_CAP_RELEASE 0x00000001UL
#define MEM_ALLOC_CAP_COALESCE 0x00000002UL
#define MEM_ALLOC_CAP_STATS 0x00000004UL
#define MEM_ALLOC_CAP_ALIGN 0x00000008UL

#define MEM_MIN_ALIGN (alignof(max_align_t))
#define MEM_ALLOC_ROUND(v, a) (((uintptr_t)v + (a - 1)) & ~(a - 1))
#define MEM_ALLOC_ALIGN(v, a) ((void *)MEM_ALLOC_ROUND(v, a))

struct mem_allocator_ops;

struct mem_allocator
{
	struct mem_allocator *upstream;
	unsigned long flags;
	const struct mem_allocator_ops *ops;
};

struct mem_allocator_ops
{
	void *(*allocate)(struct mem_allocator *allocator, size_t size, size_t alignment);
	void (*release)(struct mem_allocator *allocator, void *ptr, size_t size, size_t alignment);
	void (*destroy)(struct mem_allocator *allocator);
	size_t (*used)(const struct mem_allocator *allocator);
	size_t (*available)(const struct mem_allocator *allocator);
};

struct mem_allocator *mem_get_default(void);
void mem_set_default(struct mem_allocator *allocator);

int mem_allocator_init(struct mem_allocator *allocator, struct mem_allocator *upstream, const struct mem_allocator_ops *ops, unsigned long capabilities);

static inline void mem_destroy(struct mem_allocator *allocator)
{
	assert(allocator != 0 && allocator->ops != 0 && allocator->ops->destroy != 0);

	allocator->ops->destroy(allocator);
}

static inline struct mem_allocator *mem_get_upstream(const struct mem_allocator *allocator)
{
	assert(allocator != 0 && allocator->ops != 0);

	return allocator->upstream;
}

static inline bool mem_has_capability(struct mem_allocator *allocator, unsigned long capabilities)
{
	assert(allocator != 0);

	return (allocator->flags & capabilities) != 0;
}

static inline void *mem_allocate(struct mem_allocator *allocator, size_t size, size_t alignment)
{
	assert(allocator != 0 && allocator->ops != 0 && allocator->ops->allocate != 0);

	return allocator->ops->allocate(allocator, size, alignment);
}

static inline void mem_release(struct mem_allocator *allocator, void *ptr, size_t size, size_t alignment)
{
	assert(allocator != 0 && allocator->ops != 0 && allocator->ops->release != 0);

	allocator->ops->release(allocator, ptr, size, alignment);
}

#endif
