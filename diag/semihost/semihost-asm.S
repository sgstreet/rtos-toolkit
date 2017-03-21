/*
 * Copyright (C) 2017 Red Rocket Computing, LLC
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * semihost-asm.c
 *
 * Created on: Mar 16, 2017
 *     Author: Stephen Street (stephen@redrocketcomputing.com)
 */

#include <asm.h>

declare_function semihost_call, .text
	.fnstart
	.cantunwind

	push	{r4,r5,r6,r7,r8,r9,r10,r11,lr}
	bkpt	0xab
	pop		{r4,r5,r6,r7,r8,r9,r10,r11,pc}

	.fnend
	.pool
	.size	semihost_call, . - semihost_call
