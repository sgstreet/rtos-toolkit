/*
 * Copyright (C) 2017 Red Rocket Computing, LLC
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * blink.c
 *
 * Created on: Mar 8, 2017
 *     Author: Stephen Street (stephen@redrocketcomputing.com)
 */

#include <diag/diag.h>
#include <board/board.h>

int main(int argc, char **argv)
{
	diag_printf("Hello from semihosting\n");

	/* Infinite loop */
	while (1)
	{
		board_delay(100);
		board_puts("toggling 0");
		board_led_toggle(0);
		board_delay(100);
		board_puts("toggling 1");
		board_led_toggle(1);
	}

	return 0;
}

