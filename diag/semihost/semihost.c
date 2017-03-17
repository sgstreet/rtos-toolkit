/*
 * Copyright (C) 2017 Red Rocket Computing, LLC
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * semihost.c
 *
 * Created on: Mar 16, 2017
 *     Author: Stephen Street (stephen@redrocketcomputing.com)
 */
#include <string.h>

#include <cmsis.h>
#include <diag/semihost.h>

int semihost_is_attached(void)
{
	return (CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk) != 0;
}

int semihost_open(const char *path, uint32_t flags)
{
	uint32_t args[3] = { (uint32_t)path, flags, strlen(path) };

	return semihost_call(SEMIHOST_SYS_OPEN, args);
}

int semihost_close(int handle)
{
	return semihost_call(SEMIHOST_SYS_CLOSE, (void *)handle);
}

int semihost_read(int handle, void *buffer, size_t count)
{
	uint32_t args[3] = { handle, (uint32_t)buffer, count };

	return semihost_call(SEMIHOST_SYS_READ, args);
}

int semihost_write(int handle, const void *buffer, size_t count)
{
	uint32_t args[3] = { handle, (uint32_t)buffer, count };

	return semihost_call(SEMIHOST_SYS_WRITE, args);
}

void semihost_puts(const char *s)
{
	(void)semihost_call(SEMIHOST_SYS_WRITE0, (void *)s);
}

int semihost_getc(void)
{
	return semihost_call(SEMIHOST_SYS_READC, 0);
}

void semihost_putc(int c)
{
	(void)semihost_call(SEMIHOST_SYS_WRITEC, (void *)c);
}

int semihost_is_tty(int handle)
{
	return semihost_call(SEMIHOST_SYS_ISTTY, (void *)handle);
}

int semihost_errno(void)
{
	return semihost_call(SEMIHOST_SYS_ERRNO, 0);
}

