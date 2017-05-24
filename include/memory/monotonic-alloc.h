/*
 * Copyright (C) 2017 Red Rocket Computing, LLC
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * monotonic-alloc.h
 *
 * Created on: Apr 24, 2017
 *     Author: Stephen Street (stephen@redrocketcomputing.com)
 */

#ifndef MEMORY_MONOTONIC_ALLOC_H_
#define MEMORY_MONOTONIC_ALLOC_H_

#include <stdbool.h>

#include <memory/mem-alloc.h>

struct monotonic_allocator
{
	void *start;
	void *current;
	size_t size;
	bool allocated;
	struct mem_allocator parent;
};

int monotonic_allocator_init(struct monotonic_allocator *allocator, struct mem_allocator *upstream, void *memory, size_t size);
struct mem_allocator *monotonic_allocator_create(struct mem_allocator *upstream, void *memory, size_t size);

#endif /* MEMORY_MONOTONIC_ALLOC_H_ */
