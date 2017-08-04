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


#ifndef PREEMPT_SCHED_H_
#define PREEMPT_SCHED_H_

#include <assert.h>
#include <stdatomic.h>

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

#define SCHEDULER_MAX_MCU_PRIORITY 0x0
#define SCHEDULER_MIN_MCU_PRIORITY ((1 << SCHEDULER_PRIOR_BITS) - 1)

#define SCHEDULER_PENDSV_PRIORITY (SCHEDULER_MIN_MCU_PRIORITY)
#define SCHEDULER_SVC_PRIORITY (SCHEDULER_MIN_MCU_PRIORITY - 1)
#define SCHEDULER_SYSTICK_PRIORITY (SCHEDULER_MAX_MCU_PRIORITY + 2)

#define SCHEDULER_INT_MAX_PRIORITY (SCHEDULER_SYSTICK_PRIORITY + 1)
#define SCHEDULER_INT_MIN_PRIORITY (SCHEDULER_SVC_PRIORITY - 1)

#define SCHEDULER_HARDINT_MAX_PRIORITY (SCHEDULER_MAX_MCU_PRIORITY)
#define SCHEDULER_HARDINT_MIN_PRIORITY (SCHEDULER_SYSTICK_PRIORITY - 1)

#define SCHEDULER_NUM_TASK_PRIORITIES 32
#define SCHEDULER_MAX_TASK_PRIORITY 0
#define SCHEDULER_MIN_TASK_PRIORITY (SCHEDULER_NUM_TASK_PRIORITIES - 1)

#define SCHEDULER_MAX_MONITORS 50

#define sched_container_of(ptr, type, member) ({ \
        const typeof(((type *)0)->member) *__mptr = (ptr);    \
        (type *)((char *)__mptr - offsetof(type, member));})

#define sched_container_of_or_null(ptr, type, member) ({ \
        const typeof(((type *)0)->member) *__mptr = (ptr);    \
        __mptr ? (type *)((char *)__mptr - offsetof(type, member)) : 0;})

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

struct sched_list
{
	struct sched_list *next;
	struct sched_list *prev;
};

struct sched_queue
{
	unsigned long mask;
	struct sched_list buckets[SCHEDULER_NUM_TASK_PRIORITIES];
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

struct scheduler_timer
{
	unsigned long long expires;
	void (*expired)(void *context);
	void *context;
	struct sched_list timer_node;
};

struct task_descriptor
{
	char name[TASK_NAME_LEN];
	task_entry_point_t entry_point;
	void *context;
	unsigned int flags;
	unsigned int stack_size;
	void *stack;
};

struct task_control_block
{
	/* This must be the first field, PendSV depends on it */
	struct scheduler_frame *psp;

	const struct task_descriptor *task_descriptor;

	enum task_state state;
	int return_code;
	unsigned long events;
	unsigned long base_priority;
	unsigned long current_priority;
	unsigned long long slice_expires;

	struct scheduler_timer timer;

	struct sched_queue join_queue;

	struct sched_queue *current_queue;
	struct sched_list queue_node;

	struct sched_list scheduler_node;
};

struct monitor
{
	atomic_long value;

	struct task_control_block *owner;
	struct sched_queue lockers;
	struct sched_queue waiters;

	struct sched_list scheduler_node;
};

struct scheduler
{
	/* This must be the first field, PendSV depends on it */
	struct task_control_block *current;

	struct scheduler_frame *initial_frame;

	struct sched_queue ready_queue;
	struct sched_queue suspended_queue;

	struct sched_list timers;
	struct sched_list tasks;
	struct sched_list monitors;

	atomic_ullong jiffies;

	struct sched_list free_monitors;
	struct monitor available_monitors[SCHEDULER_MAX_MONITORS];
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

static inline void sched_list_init(struct sched_list *list)
{
	list->next = list;
	list->prev = list;
}

static inline bool sched_list_empty(struct sched_list *list)
{
	assert(list != 0);

	return list->next == list;
}

static inline void sched_list_insert(struct sched_list *node, struct sched_list *first, struct sched_list *second)
{
	assert(node != 0 && first != 0 && second != 0);

	second->prev = node;
	node->next = second;
	node->prev = first;
	first->next = node;
}

static inline void sched_list_remove(struct sched_list *node)
{
	assert(node != 0);

	node->next->prev = node->prev;
	node->prev->next = node->next;
	node->next = node;
	node->prev = node;;
}

static inline void sched_list_push(struct sched_list *list, struct sched_list *node)
{
	assert(list != 0 && node != 0);

	sched_list_insert(node, list->prev, list);
}

static inline struct sched_list *sched_list_pop(struct sched_list *list)
{
	assert(list != 0);

	struct sched_list *node = list->next;

	if (node == list)
		return 0;

	sched_list_remove(node);

	return node;
}

static inline void sched_queue_init(struct sched_queue *queue)
{
	assert(queue != 0);

	queue->mask = 0;
	for (int i = 0; i < SCHEDULER_NUM_TASK_PRIORITIES; ++i) {
		queue->buckets[i].next = &queue->buckets[i];
		queue->buckets[i].prev = &queue->buckets[i];
	}
}

static inline void sched_queue_push(struct sched_queue *queue, struct sched_list *node, unsigned long priority)
{
	assert(queue != 0 && node != 0 && priority >= 0 && priority < SCHEDULER_NUM_TASK_PRIORITIES);

	sched_list_push(&queue->buckets[priority], node);

	queue->mask |= (1 << (SCHEDULER_NUM_TASK_PRIORITIES - priority - 1));
}

static inline struct sched_list *sched_queue_pop(struct sched_queue *queue)
{
	assert(queue != 0);

	unsigned long priority = __builtin_clzl(queue->mask);
	if (priority >= SCHEDULER_NUM_TASK_PRIORITIES)
		return 0;

	struct sched_list *node = sched_list_pop(&queue->buckets[priority]);
	if (sched_list_empty(&queue->buckets[priority]))
		queue->mask &= ~(1 << (SCHEDULER_NUM_TASK_PRIORITIES - priority - 1));

	return node;
}

static inline void sched_queue_reprioritize(struct sched_queue *queue, struct sched_list *node, unsigned long old_priority, unsigned long new_priority)
{
	assert(queue != 0 && node != 0 && old_priority >= 0 && old_priority < SCHEDULER_NUM_TASK_PRIORITIES && new_priority >= 0 && new_priority < SCHEDULER_NUM_TASK_PRIORITIES);

	sched_list_remove(node);
	if (sched_list_empty(&queue->buckets[old_priority]))
		queue->mask &= ~(1 << (SCHEDULER_NUM_TASK_PRIORITIES - old_priority - 1));

	sched_queue_push(queue, node, new_priority);
}

#define sched_list_pop_entry(list, type, member) sched_container_of_or_null(sched_list_pop(list), type, member)
#define sched_list_entry(ptr, type, member) sched_container_of_or_null(ptr, type, member)
#define sched_list_first_entry(list, type, member) sched_list_entry((list)->next, type, member)
#define sched_list_last_entry(list, type, member) sched_list_entry((list)->prev, type, member)

int scheduler_run(struct scheduler *new_scheduler, int count, const struct task_descriptor *descriptors);
int scheduler_is_running(void);

unsigned long scheduler_create(const struct task_descriptor *descriptor);
unsigned long scheduler_get_tid(void);
unsigned long scheduler_set_priority(unsigned long tid, unsigned long priority);
unsigned long scheduler_get_priority(unsigned long tid);
int scheduler_join(unsigned long tid, int *return_code, unsigned long msecs);

int scheduler_yield(void);
int scheduler_sleep(unsigned long msec);

int scheduler_suspend(void);
int scheduler_resume(unsigned long tid);

unsigned long scheduler_event_wait(unsigned long mask, unsigned long msecs);
int scheduler_event_notify(unsigned long tid, unsigned long events);

unsigned long scheduler_monitor_allocate(void);
int scheduler_monitor_release(unsigned long mid);
int scheduler_monitor_lock(unsigned long mid, unsigned long msecs);
int scheduler_monitor_unlock(unsigned long mid);
int scheduler_monitor_wait(unsigned long mid, unsigned long msecs);
int scheduler_monitor_notify(unsigned long mid, bool all);

#endif
