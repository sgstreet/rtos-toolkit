/*
 * Copyright (C) 2017 Red Rocket Computing, LLC
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * brk-alloc.h
 *
 * Created on: Mar 28, 2017
 *     Author: Stephen Street (stephen@redrocketcomputing.com)
 */

#ifndef MEMORY_BRK_ALLOC_H_
#define MEMORY_BRK_ALLOC_H_

#include <stdlib.h>
#include <stddef.h>

/* Comment out to quietly fail on any call to release or free */
#define brk_abort() abort()

struct brk_allocator
{
	void *start;
	void *end;
};

int brk_alloc_init(struct brk_allocator *allocator, void *start, void *end);
struct brk_allocator *brk_alloc_create(void *start, void *end);
int brk_alloc_destroy(struct brk_allocator *allocator);

void *brk_alloc(struct brk_allocator *allocator, size_t size);
void brk_release(struct brk_allocator *allocator, void *ptr, size_t size);
void brk_free(struct brk_allocator *allocator, void *ptr);

#endif
