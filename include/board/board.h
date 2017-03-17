/*
 * Copyright (C) 2017 Red Rocket Computing, LLC
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * board.h
 *
 * Created on: Mar 8, 2017
 *     Author: Stephen Street (stephen@redrocketcomputing.com)
 */


#ifndef BOARD_H_
#define BOARD_H_

void board_delay(unsigned long msecs);

int board_get_num_leds(void);
int board_led_on(int led);
int board_led_off(int led);
int board_led_toggle(int led);

int board_getchar(void);
int board_putchar(int c);
int board_puts(const char *s);

#endif
