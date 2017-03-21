/*
 * Copyright (C) 2017 Red Rocket Computing, LLC
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * scheduler.h
 *
 * Created on: Feb 13, 2017
 *     Author: Stephen Street (stephen@redrocketcomputing.com)
 */


#ifndef SIMPLE_SCHED_H_
#define SIMPLE_SCHED_H_

#include <util/linked-list.h>

#define TASK_NAME_LEN 32

#define SCHEDULER_SAVE_FRAME_LINKAGE_Msk 0x20
#define SCHEDULER_UPDATE_STACK_LINKAGE_Msk 0x40
#define SCHEDULER_SVC_CODE(SVC_CODE) SVC_Handler_ ## SVC_CODE
#define SCHEDULER_DECLARE_SVC(SVC_CODE, SVC_FUNC) void SCHEDULER_SVC_CODE(SVC_CODE)(void) __attribute__ ((alias(#SVC_FUNC)));

#define SCHEDULER_START_SVC 0
#define SCHEDULER_YIELD_SVC 1
#define SCHEDULER_TERMINATE_SVC 2
#define SCHEDULER_SUSPEND_SVC 3
#define SCHEDULER_WAIT_SVC 4
#define SCHEDULER_JOIN_SVC 5

#define SCHEDULER_PRIOR_BITS 0x00000004UL
#define SCHEDULER_PRIGROUP 0x00000003UL
#define SCHEDULER_MAX_HARD_PRIORITY 0x0
#define SCHEDULER_MIN_HARD_PRIORITY ((1 << SCHEDULER_PRIOR_BITS) - 1)

#define SCHEDULER_PENDSV_PRIORITY (SCHEDULER_MIN_HARD_PRIORITY)
#define SCHEDULER_SVC_PRIORITY (SCHEDULER_MIN_HARD_PRIORITY - 1)
#define SCHEDULER_SYSTICK_PRIORITY (SCHEDULER_MIN_HARD_PRIORITY - 2)

struct exception_frame
{
	uint32_t r0;
	uint32_t r1;
	uint32_t r2;
	uint32_t r3;
	uint32_t r12;
	uint32_t lr;
	uint32_t pr;
	uint32_t psr;

	uint32_t s0;
	uint32_t s1;
	uint32_t s2;
	uint32_t s3;
	uint32_t s4;
	uint32_t s5;
	uint32_t s6;
	uint32_t s7;
	uint32_t s8;
	uint32_t s9;
	uint32_t s10;
	uint32_t s11;
	uint32_t s12;
	uint32_t s13;
	uint32_t s14;
	uint32_t s15;
	uint32_t fpscr;
};

struct scheduler_frame
{
	uint32_t exec_return;
	uint32_t control;
	uint32_t basepri;

	uint32_t r4;
	uint32_t r5;
	uint32_t r6;
	uint32_t r7;
	uint32_t r8;
	uint32_t r9;
	uint32_t r10;
	uint32_t r11;

	uint32_t s16;
	uint32_t s17;
	uint32_t s18;
	uint32_t s19;
	uint32_t s20;
	uint32_t s21;
	uint32_t s22;
	uint32_t s23;
	uint32_t s24;
	uint32_t s25;
	uint32_t s26;
	uint32_t s27;
	uint32_t s28;
	uint32_t s29;
	uint32_t s30;
	uint32_t s31;

	uint32_t r0;
	uint32_t r1;
	uint32_t r2;
	uint32_t r3;
	uint32_t r12;
	uint32_t lr;
	uint32_t pc;
	uint32_t psr;

	uint32_t s0;
	uint32_t s1;
	uint32_t s2;
	uint32_t s3;
	uint32_t s4;
	uint32_t s5;
	uint32_t s6;
	uint32_t s7;
	uint32_t s8;
	uint32_t s9;
	uint32_t s10;
	uint32_t s11;
	uint32_t s12;
	uint32_t s13;
	uint32_t s14;
	uint32_t s15;
	uint32_t fpscr;
};

enum task_state
{
	TASK_DEAD = 0,
	TASK_RUNNING,
	TASK_READY,
	TASK_BLOCKED,
	TASK_SUSPENDED,
	TASK_SLEEPING,
};

typedef int (*task_entry_point_t)(unsigned int tid, void *context);

struct task_descriptor
{
	char name[TASK_NAME_LEN];
	task_entry_point_t entry_point;
	void *context;
	unsigned int flags;
	unsigned int stack_size;
	void *stack;
};

struct scheduler_timer
{
	uint64_t expires;
	void (*expired)(void *context);
	void *context;
	struct list_node timer_node;
};

struct task_control_block
{
	struct scheduler_frame *psp;
	enum task_state state;
	int return_code;
	unsigned long events;

	struct scheduler_timer timer;

	const struct task_descriptor *task_descriptor;

	struct linked_list join_queue;
	struct list_node queue_node;
	struct list_node scheduler_node;
};

struct scheduler
{
	/* This must be the first field, PendSV depends on it */
	struct task_control_block *current;

	struct scheduler_frame *initial_frame;

	struct linked_list ready_queue;
	struct linked_list suspended_queue;
	struct linked_list timers;
	struct linked_list tasks;

	unsigned long jiffies;
};

static inline int __attribute__((always_inline)) svc_call0(const uint8_t code)
{
	int result;

	asm volatile
	(
		"svc 	%[code]				\n\t"
		"mov	%[result], r0		\n\t"
		: [result] "=r" (result) : [code] "I" (code)
	);

	return result;
}

static inline int __attribute__((always_inline)) svc_call1(uint8_t code, uint32_t arg0)
{
	int result;

	asm volatile
	(
		"mov	r0, %[arg0]			\n\t"
		"svc 	%[code]				\n\t"
		"mov	%[result], r0		\n\t"
		: [result] "=r" (result) : [code] "I" (code), [arg0] "r" (arg0) : "r0"
	);

	return result;
}

static inline int __attribute__((always_inline)) svc_call2(uint8_t code, uint32_t arg0, uint32_t arg1)
{
	int result;

	asm volatile
	(
		"mov	r0, %[arg0]			\n\t"
		"mov	r1, %[arg1]			\n\t"
		"svc 	%[code]				\n\t"
		"mov	%[result], r0		\n\t"
		: [result] "=r" (result) : [code] "I" (code), [arg0] "r" (arg0), [arg1] "r" (arg1) : "r0", "r1"
	);

	return result;
}

static inline int __attribute__((always_inline)) svc_call3(uint8_t code, uint32_t arg0, uint32_t arg1, uint32_t arg2)
{
	int result;

	asm volatile
	(
		"mov	r0, %[arg0]			\n\t"
		"mov	r1, %[arg1]			\n\t"
		"mov	r2, %[arg2]			\n\t"
		"svc 	%[code]				\n\t"
		"mov	%[result], r0		\n\t"
		: [result] "=r" (result) : [code] "I" (code), [arg0] "r" (arg0), [arg1] "r" (arg1), [arg2] "r" (arg2) : "r0", "r1", "r2"
	);

	return result;
}

static inline int __attribute__((always_inline)) svc_call4(uint8_t code, uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3)
{
	int result;

	asm volatile
	(
		"mov	r0, %[arg0]			\n\t"
		"mov	r1, %[arg1]			\n\t"
		"mov	r2, %[arg2]			\n\t"
		"mov	r3, %[arg3]			\n\t"
		"svc 	%[code]				\n\t"
		"mov	%[result], r0		\n\t"
		: [result] "=r" (result) : [code] "I" (code), [arg0] "r" (arg0), [arg1] "r" (arg1), [arg2] "r" (arg2), [arg3] "r" (arg3): "r0", "r1", "r2", "r3"
	);

	return result;
}

static inline void __attribute__((always_inline)) scheduler_disable_interrupts(void)
{
	asm volatile("cpsid i" : : : "memory");
}

static inline void __attribute__((always_inline)) scheduler_enable_interrupts(void)
{
	asm volatile("cpsie i" : : : "memory");
}

int scheduler_run(struct scheduler *new_scheduler, int count, const struct task_descriptor *descriptors);
unsigned long scheduler_create(const struct task_descriptor *descriptor);

unsigned long scheduler_get_tid(void);

int scheduler_is_running(void);
int scheduler_yield(void);
int scheduler_sleep(unsigned long msec);

int scheduler_suspend(void);
int scheduler_resume(unsigned long tid);

unsigned long scheduler_wait_event(unsigned long mask, unsigned long msecs);
int scheduler_notify_event(unsigned long tid, unsigned long events);
int scheduler_join(unsigned long tid, int *return_code, unsigned long msecs);

#endif
