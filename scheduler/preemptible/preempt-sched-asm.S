/*
 * Copyright (C) 2017 Red Rocket Computing, LLC
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * scheduler-asm.S
 *
 * Created on: Feb 9, 2017
 *     Author: Stephen Street (stephen@redrocketcomputing.com)
 */

#include <asm.h>

	.syntax	unified

#if __ARM_ARCH == 7
	.arch	armv7-m
#else
	.arch	armv6-m
#endif

/* Global services jump table, patterned after the CMSIS standard interrupt table */
	.section .data
	.align	2
	.global	services
services:
	.long	SVC_Handler_0
	.long	SVC_Handler_1
	.long	SVC_Handler_2
	.long	SVC_Handler_3
	.long	SVC_Handler_4
	.long	SVC_Handler_5
	.long	SVC_Handler_6
	.long	SVC_Handler_7
	.long	SVC_Handler_8
	.long	SVC_Handler_9
	.long	SVC_Handler_10
	.long	SVC_Handler_11
	.long	SVC_Handler_12
	.long	SVC_Handler_13
	.long	SVC_Handler_14
	.long	SVC_Handler_15
	.long	SVC_Handler_16
	.long	SVC_Handler_17
	.long	SVC_Handler_18
	.long	SVC_Handler_19
	.long	SVC_Handler_20
	.long	SVC_Handler_21
	.long	SVC_Handler_22
	.long	SVC_Handler_23
	.long	SVC_Handler_24
	.long	SVC_Handler_25
	.long	SVC_Handler_26
	.long	SVC_Handler_27
	.long	SVC_Handler_28
	.long	SVC_Handler_29
	.long	SVC_Handler_30
	.long	SVC_Handler_31
	.size	services, . - services

declare_function SVC_Handler, .after_vectors
	.fnstart

	/* Load pointer to the exception frame into r0 */
	tst		lr, #4
	ite		eq
	mrseq	r0, msp
	mrsne	r0, psp

	/* Load the service number into r12 */
	ldr		r12, [r0, #24]
	ldrb	r12, [r12, #-2]

	/* Test the call linkage bits, if zero we should perform a fastcall */
	tst		r12, #0xe0
	bne		handle_linkage_bits

	/* Load handler address in to r1 */
	and		r12, 0x1f
	ldr		r1, =services
	ldr		r1, [r1, r12, lsl #2]

	/* Invoke the service handler who will return from the service call*/
	bx		r1

	/* Should we create the scheduler frame by storing the callee saved registers and control/exec_return */
handle_linkage_bits:
	tst		r12, #0x20
	beq		update_stack

	/* Stack the callee save floating point registers, if there is a floating point context */
	tst			lr, #0x10
	ite			eq
	vstmdbeq	r0!, {s16-s31}
	subne		r0, #64

	/* Save the callee/control register */
	mov		r1, lr
	mrs		r2, control
	mrs		r3, basepri
	stmdb	r0!, {r1-r11}

	/* Update the stack pointer used by the caller, ensure stack remains double word aligned */
update_stack:
	tst		r12, #0x40
	beq		do_svc
	sub		r0, #4
	tst		lr, #4
	ite		eq
	msreq	msp, r0
	msrne	psp, r0
	add		r0, #4

	/* Load handler address in to r1 */
do_svc:
	and		r12, 0x1f
	ldr		r1, =services
	ldr		r1, [r1, r12, lsl #2]

	/* Invoke the service handler who will return from the service call*/
	bx		r1

	.fnend
	.pool
	.size	SVC_Handler, . - SVC_Handler

declare_function PendSV_Handler, .after_vectors
	.fnstart

	/* No interrupts while switching context */
	cpsid		i

	/* If the pointer to the current task is zero, skip the scheduler frame push */
	ldr		r0,	=scheduler
	ldr		r0, [r0]
	ldr		r0, [r0, #0]
	cbz		r0, skip_save

	/* Get the active stack pointer into r0 */
	tst		lr, #4
	ite		eq
	mrseq	r0, msp
	mrsne	r0, psp

	/* Stack the callee save floating point registers, if there is a floating point context */
	tst			lr, #0x10
	ite			eq
	vstmdbeq	r0!, {s16-s31}
	subne		r0, #64

	/* Save the current context */
	mov		r1, lr
	mrs		r2, control
	mrs		r3, basepri
	stmdb	r0!, {r1-r11}

skip_save:
	bl		scheduler_switch

	/* Load the new context, which was stashed */
	ldmia	r0!, {r1-r11}
	mov		lr, r1
	msr		control, r2
	msr		basepri, r3

	/* Unstack the callee saved float point registers, if there is a float point context */
	tst			lr, #0x10
	ite			eq
	vldmiaeq	r0!, {s16-s31}
	addne		r0, #64

	/* Force instruction barrier */
	isb

	/* Set current stack pointer */
	tst		lr, #4
	ite		eq
	msreq	msp, r0
	msrne	psp, r0

good:
	/* Alway clear the pendsv pending bit */
	ldr		r0, =#0xe000ed04
	mov		r1, #0x08000000
	str		r1, [r0]

	/* Let is fly */
	cpsie	i
	bx		lr

	.fnend
	.pool
	.size	PendSV_Handler, . - PendSV_Handler


declare_weak_function Default_Service_Handler, .after_vectors
	.fnstart

	bx		lr

	.fnend
	.pool
	.size Default_Service_Handler, . - Default_Service_Handler


def_default_function SVC_Handler_0, Default_Service_Handler
def_default_function SVC_Handler_1, Default_Service_Handler
def_default_function SVC_Handler_2, Default_Service_Handler
def_default_function SVC_Handler_3, Default_Service_Handler
def_default_function SVC_Handler_4, Default_Service_Handler
def_default_function SVC_Handler_5, Default_Service_Handler
def_default_function SVC_Handler_6, Default_Service_Handler
def_default_function SVC_Handler_7, Default_Service_Handler
def_default_function SVC_Handler_8, Default_Service_Handler
def_default_function SVC_Handler_9, Default_Service_Handler
def_default_function SVC_Handler_10, Default_Service_Handler
def_default_function SVC_Handler_11, Default_Service_Handler
def_default_function SVC_Handler_12, Default_Service_Handler
def_default_function SVC_Handler_13, Default_Service_Handler
def_default_function SVC_Handler_14, Default_Service_Handler
def_default_function SVC_Handler_15, Default_Service_Handler
def_default_function SVC_Handler_16, Default_Service_Handler
def_default_function SVC_Handler_17, Default_Service_Handler
def_default_function SVC_Handler_18, Default_Service_Handler
def_default_function SVC_Handler_19, Default_Service_Handler
def_default_function SVC_Handler_20, Default_Service_Handler
def_default_function SVC_Handler_21, Default_Service_Handler
def_default_function SVC_Handler_22, Default_Service_Handler
def_default_function SVC_Handler_23, Default_Service_Handler
def_default_function SVC_Handler_24, Default_Service_Handler
def_default_function SVC_Handler_25, Default_Service_Handler
def_default_function SVC_Handler_26, Default_Service_Handler
def_default_function SVC_Handler_27, Default_Service_Handler
def_default_function SVC_Handler_28, Default_Service_Handler
def_default_function SVC_Handler_29, Default_Service_Handler
def_default_function SVC_Handler_30, Default_Service_Handler
def_default_function SVC_Handler_31, Default_Service_Handler

