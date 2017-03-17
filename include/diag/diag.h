/*
 * Copyright (C) 2017 Red Rocket Computing, LLC
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * diag.h
 *
 * Created on: Mar 8, 2017
 *     Author: Stephen Street (stephen@redrocketcomputing.com)
 */

#include <stdarg.h>

#ifndef DIAG_H_
#define DIAG_H_

#define DIAG_BUFFER_SIZE 256

extern int diag_puts(const char *s);

int diag_printf(const char *fmt, ...);

#endif
