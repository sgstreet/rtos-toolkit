/*
 * Copyright (C) 2017 Red Rocket Computing, LLC
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * mem-alloc.c
 *
 * Created on: Apr 22, 2017
 *     Author: Stephen Street (stephen@redrocketcomputing.com)
 */

#include <memory/mem-alloc.h>

static struct mem_allocator *default_allocator = 0;

struct mem_allocator *mem_get_default(void)
{
	return default_allocator;
}

void mem_set_default(struct mem_allocator *allocator)
{
	default_allocator = allocator;
}

int mem_allocator_init(struct mem_allocator *allocator, struct mem_allocator *upstream, const struct mem_allocator_ops *ops, unsigned long capabilities)
{
	assert(allocator != 0 && ops != 0);

	allocator->upstream = upstream;
	allocator->ops = ops;
	allocator->flags = capabilities;

	return 0;
}



