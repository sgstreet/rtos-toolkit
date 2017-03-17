/*
 * Copyright (C) 2017 Red Rocket Computing, LLC
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * startup.c
 *
 * Created on: Mar 16, 2017
 *     Author: Stephen Street (stephen@redrocketcomputing.com)
 */

#include <init/init-sections.h>
#include <stdint.h>
#include <sys/types.h>


extern void __initialize_args(int*, char***);
extern int _main(int argc, char* argv[]);
extern int main(int argc, char* argv[]);
extern void __attribute__((noreturn)) _exit(int);

void _start(void);

char* __env[1] = { 0 };
char** environ = __env;

static char __name[] = "";
static char *__argv[2] = { __name, 0 };

void __attribute__((weak)) __initialize_args(int *argc, char***argv)
{
	*argc = 1;
	*argv = &__argv[0];
}

int __attribute__((weak)) _main(int argc, char **argv)
{
	return main(argc, argv);
}

void __attribute__ ((section(".after_vectors"), noreturn, weak)) _start(void)
{
	int argc;
	char **argv;
	int result;

	/* Execute the preinit array */
	run_init_array(__preinit_array_start, __preinit_array_end);

	/* Setup the program arguments */
	__initialize_args(&argc, &argv);

	/* Execute the init array */
	run_init_array(__init_array_start, __init_array_end);

	/* Forward to the next stage */
	result = _main(argc, argv);

	/* Execute the fini array */
	run_init_array(__fini_array_start, __fini_array_end);

	/* Forward to the exist routine with the result */
	_exit(result);

	/* We should never get here */
	while (1);
}
