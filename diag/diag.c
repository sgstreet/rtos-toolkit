/*
 * Copyright (C) 2017 Red Rocket Computing, LLC
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * diag.c
 *
 * Created on: Mar 8, 2017
 *     Author: Stephen Street (stephen@redrocketcomputing.com)
 */

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>

#include <diag/diag.h>

static char diag_buffer[DIAG_BUFFER_SIZE];

int diag_printf(const char *fmt, ...)
{
	va_list ap;

	/* Initialize the variable arguments */
	va_start(ap, fmt);

	/* Visually mark buffer truncation */
	diag_buffer[DIAG_BUFFER_SIZE - 4] = '.';
	diag_buffer[DIAG_BUFFER_SIZE - 3] = '.';
	diag_buffer[DIAG_BUFFER_SIZE - 2] = '.';
	diag_buffer[DIAG_BUFFER_SIZE - 1] = 0;

	/* Build the buffer */
	ssize_t amount = vsnprintf(diag_buffer, DIAG_BUFFER_SIZE - 3, fmt, ap);
	if (amount > DIAG_BUFFER_SIZE - 3)
		amount = DIAG_BUFFER_SIZE;

	/* Forward to link time select output function */
	diag_puts(diag_buffer);

	/* Clean up */
	va_end(ap);

	/* Return the amount of data actually sent */
	return amount;
}


