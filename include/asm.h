/*
 * Copyright (C) 2017 Red Rocket Computing, LLC
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * asm.h
 *
 * Created on: Mar 16, 2017
 *     Author: Stephen Street (stephen@redrocketcomputing.com)
 */

#ifndef ASM_H_
#define ASM_H_

#ifdef __ASSEMBLER__

	.syntax	unified

#if __ARM_ARCH == 7
	.arch	armv7-m
#else
	.arch	armv6-m
#endif

/* Macro to declare function */
.macro declare_function function_name, section_name
	.L0_\function_name:
	.asciz "\function_name"
	.align 2
	.L1_\function_name:
	.word 0xff000000 + (.L1_\function_name - .L0_\function_name)
	.text
	.thumb_func
	.global \function_name
	.type \function_name, %function
	.section \section_name\().\function_name
	\function_name:
.endm

/* Macro to declare function */
.macro declare_weak_function function_name, section_name
	.L0_\function_name:
	.asciz "\function_name"
	.align 2
	.L1_\function_name:
	.word 0xff000000 + (.L1_\function_name - .L0_\function_name)
	.text
	.thumb_func
	.weak \function_name
	.type \function_name, %function
	.section \section_name\().\function_name
	\function_name:
.endm

/* Macro to export weak alias to a default function */
.macro	def_default_function handler_name default_function
	.weak	\handler_name
	.set	\handler_name, \default_function
.endm

#endif

#endif
