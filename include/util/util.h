/*
 * Copyright (C) 2017 Red Rocket Computing, LLC
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * util.h
 *
 * Created on: May 3, 2017
 *     Author: Stephen Street (stephen@redrocketcomputing.com)
 */

#ifndef UTIL_UTIL_H_
#define UTIL_UTIL_H_

#define min(a, b) ({typeof(a) __a = (a); typeof(b) __b = (b); __a < __b ? __a : __b;})
#define max(a, b) ({typeof(a) __a = (a); typeof(b) __b = (b); __a > __b ? __a : __b;})

#endif /* UTIL_UTIL_H_ */
