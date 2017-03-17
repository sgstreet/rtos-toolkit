/*
 * Copyright (C) 2017 Red Rocket Computing, LLC
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * _main.c
 *
 * Created on: Mar 9, 2017
 *     Author: Stephen Street (stephen@redrocketcomputing.com)
 */

#include <stdbool.h>
#include <diag/diag.h>
#include <init/init-sections.h>

extern int main(int argc, char **argv);

int _main(int argc, char **argv)
{
	diag_printf("__vectors: %p size: %lu\n", &__vectors, &__vectors_size);
	diag_printf("__inits: %p, size: %lu\n", &__inits, &__inits_size);
	diag_printf("__text: %p, size: %lu\n", &__text, &__text_size);
	diag_printf("__rodata: %p, size: %lu\n", &__rodata, &__rodata_size);
	diag_printf("__unwind_info: %p, size: %lu\n", &__unwind_info, &__unwind_info_size);
	diag_printf("__data: %p, size: %lu loaded at %p\n", &__data, &__data_size, &__data_start__);
	diag_printf("__bss: %p, size: %lu loaded at %p\n", &__bss, &__bss_size, &__bss_start__);
	diag_printf("__heap: %p, size: %lu\n", &__heap, &__heap_size);
	diag_printf("__stack: %p\n", &__stack);

	int count = 1;
	while (true) {
		diag_printf("Running CoreMark: %d\n", count++);
		main(argc, argv);
	}
}


