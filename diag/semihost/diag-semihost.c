/*
 * Copyright (C) 2017 Red Rocket Computing, LLC
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * diag-semihost.c
 *
 * Created on: Mar 8, 2017
 *     Author: Stephen Street (stephen@redrocketcomputing.com)
 */

#include <string.h>

#include <diag/diag.h>
#include <diag/semihost.h>

int diag_puts(const char *s)
{
	if (semihost_is_attached()) {
		semihost_puts(s);
		return strlen(s);
	}
	return 0;
}


