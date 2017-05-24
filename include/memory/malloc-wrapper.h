/*
 * Copyright (C) 2017 Red Rocket Computing, LLC
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * malloc-wrapper.h
 *
 * Created on: May 3, 2017
 *     Author: Stephen Street (stephen@redrocketcomputing.com)
 */

#ifndef MEMORY_MALLOC_WRAPPER_H_
#define MEMORY_MALLOC_WRAPPER_H_

#include <memory/mem-alloc.h>

int malloc_init(struct mem_allocator *upstream);

#endif /* MEMORY_MALLOC_WRAPPER_H_ */
