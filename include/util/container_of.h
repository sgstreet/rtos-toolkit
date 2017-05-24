/*
 * Copyright (C) 2017 Red Rocket Computing, LLC
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * container_of.h
 *
 * Created on: Apr 27, 2017
 *     Author: Stephen Street (stephen@redrocketcomputing.com)
 */

#ifndef UTIL_CONTAINER_OF_H_
#define UTIL_CONTAINER_OF_H_

#define container_of(ptr, type, member) ({ \
        const typeof(((type *)0)->member) *__mptr = (ptr);    \
        (type *)((char *)__mptr - offsetof(type, member));})

#define container_of_or_null(ptr, type, member) ({ \
        const typeof(((type *)0)->member) *__mptr = (ptr);    \
        __mptr ? (type *)((char *)__mptr - offsetof(type, member)) : 0;})


#endif /* UTIL_CONTAINER_OF_H_ */
