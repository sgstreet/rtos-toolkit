/*
 * Copyright (C) 2017 Red Rocket Computing, LLC
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * scheduler-fifo.c
 *
 * Created on: Feb 13, 2017
 *     Author: Stephen Street (stephen@redrocketcomputing.com)
 */

#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#include <cmsis.h>

#include <scheduler/preempt-sched.h>

#define ALIGNMENT_ROUND(TYPE, BYTES) ((sizeof(TYPE) + (BYTES - 1)) & ~(BYTES - 1))

void SysTick_Handler(void);
void scheduler_start_svc(struct scheduler_frame *frame);
void scheduler_yield_svc(struct exception_frame *);
void scheduler_terminate_svc(struct exception_frame *frame);
void scheduler_suspend_svc(struct scheduler_frame *frame);
void scheduler_wait_svc(struct scheduler_frame *frame);
void scheduler_join_svc(struct scheduler_frame *frame);
struct scheduler_frame *scheduler_switch(struct scheduler_frame *current_frame);

struct scheduler *scheduler = 0;

static inline void __attribute__((always_inline)) request_context_switch(void)
{
	/* Pend the pendsv interrupt */
	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;

	/* Make sure this is visable, not require but architecturally recommended */
	__DSB();
}

static inline bool __attribute__((always_inline)) is_interrupt_context(void)
{
	uint32_t result;

	asm volatile ("mrs %0, ipsr" : "=r" (result));

	return result != 0;
}

static inline bool __attribute__((always_inline)) is_svc_context(void)
{
	uint32_t result;

	asm volatile ("mrs %0, ipsr" : "=r" (result));

	return result == 11;
}

void SysTick_Handler(void)
{
	/* Someone may and enabled us too early, ignore */
	if (!scheduler)
		return;

	/* Update the jiffies */
	++scheduler->jiffies;

	/* Do not let higher priority interrupt, bug us while adjusting the scheduling queues */
	scheduler_disable_interrupts();

	/* Notify all expired timers */
	struct scheduler_timer *expired;
	while((list_first_entry(&scheduler->timers, struct scheduler_timer, timer_node)->expires < scheduler->jiffies)) {
		expired = list_pop_entry(&scheduler->timers, struct scheduler_timer, timer_node);
		expired->expired(expired->context);
	}

	/* All good */
	scheduler_enable_interrupts();
}

static void add_timer(struct scheduler_timer *timer, uint32_t delay, void (*expired)(void *context), void *context)
{
	/* Initialize the timer */
	timer->expires = scheduler->jiffies + delay;
	timer->expired = expired;
	timer->context = context;

	/* Find the insert point */
	struct scheduler_timer *entry;
	list_for_each_entry(entry, &scheduler->timers, timer_node)
		if (entry->expires > timer->expires)
			break;

	/* Insert at the correct position, which might be the head */
	list_insert_before(&entry->timer_node, &timer->timer_node);
}

static bool delete_timer(struct scheduler_timer *timer)
{
	/* Because we run timer expired notification with interrupts enabled,
	 * we only want to remove which are still in the list */
	struct scheduler_timer *entry;
	list_for_each_entry(entry, &scheduler->timers, timer_node)
		if (entry == timer) {
			list_remove(&timer->timer_node);
			break;
		}

	/* Let call know if we successfully delete the timer */
	return timer != 0;
}

static void scheduler_wakeup(void *context)
{
	struct task_control_block *tcb = context;

	/* Remove from the whatever queue the tcb is one */
	list_remove(&tcb->queue_node);

	/* Mark as ready and set return code to timeout */
	tcb->state = TASK_READY;
	tcb->psp->r0 = (uint32_t)-ETIMEDOUT;

	/* Add to the back of the ready queue */
	list_add(&scheduler->ready_queue, &tcb->queue_node);
}

static void dispatch(struct task_control_block *tcb)
{
	/* Forward to real task entry point */
	tcb->return_code = tcb->task_descriptor->entry_point((uint32_t)tcb, tcb->task_descriptor->context);

	/* We are done, but terminate is not a public service so invoke directly and should never return */
	(void)svc_call0(SCHEDULER_TERMINATE_SVC);

	/* This should NEVER happen */
	abort();
}

void scheduler_start_svc(struct scheduler_frame *frame)
{
	assert(frame != 0);

	/* Assume every is ok */
	frame->r0 = 0;

	/* Do we have and initial task set, if no, do not start, there should be at least one task to start */
	if (!scheduler || list_is_empty(&scheduler->ready_queue))	{
		frame->r0 = (uint32_t)-EINVAL;
		return;
	}

	/* Have we already be started? */
	if (scheduler->initial_frame) {
		frame->r0 = (uint32_t)-EBUSY;
		return;
	}

	/* Ensure that there is not current frame for pendsv to push */
	scheduler->current = 0;

	/* Save the initial frame so we can exit the scheduler when there is no available work */
	scheduler->initial_frame = frame;

	/* Pend the context switch to start running */
	request_context_switch();
}
SCHEDULER_DECLARE_SVC(SCHEDULER_START_SVC, scheduler_start_svc);

void scheduler_yield_svc(struct exception_frame __attribute__((unused)) *frame)
{
	/* Assume every is ok */
	frame->r0 = 0;

	/* Pend the context switch to switch to the next task */
	request_context_switch();
}
SCHEDULER_DECLARE_SVC(SCHEDULER_YIELD_SVC, scheduler_yield_svc);

void scheduler_terminate_svc(struct exception_frame __attribute__((unused)) *frame)
{
	/* Assume every is ok */
	frame->r0 = 0;

	/* Protect the scheduler and tcb from interrupts */
	scheduler_disable_interrupts();

	/* First we need to make all thread blocked on the join schedulable by marking as read and adding to scheduler in LIFO order */
	struct task_control_block *entry;
	while ((entry = list_pop_entry(&scheduler->current->join_queue, struct task_control_block, queue_node)) != 0) {
		entry->state = TASK_READY;
		list_add(&scheduler->ready_queue, &entry->queue_node);
	}

	/* Since this is a non-preemptable cooperative scheduler and SVC exception has higher priority than the
	 * PENDSV exception (context switching) there is no need to lock the scheduler. Since we
	 * can only terminate the ourselves, clearing the current tcb and forcing a context switch
	 * will be enough to terminate a thread */
	scheduler->current->state = TASK_DEAD;
	scheduler->current = 0;
	request_context_switch();

	/* Let the dogs out */
	scheduler_enable_interrupts();
}
SCHEDULER_DECLARE_SVC(SCHEDULER_TERMINATE_SVC, scheduler_terminate_svc);

void scheduler_suspend_svc(struct scheduler_frame *frame)
{
	unsigned long msecs = frame->r0;

	/* Assume every is ok */
	frame->r0 = 0;

	/* Protect the scheduler and tcb from interrupts */
	scheduler_disable_interrupts();

	/* We are the currently scheduled task, update the tcb psp our scheduling frame
	 * which got pushed by the SVC handler linkage control. */
	scheduler->current->psp = frame;

	/* What kind of suspend was requested? */
	if (msecs > 0) {

		/* Update the state and return, which we will to indicate a timeout */
		scheduler->current->state = TASK_SLEEPING;

		/* Add a timer, under protection from the systick */
		add_timer(&scheduler->current->timer, msecs, scheduler_wakeup, scheduler->current);

	} else
		scheduler->current->state = TASK_SUSPENDED;

	/* Add ourselves to the suspend queue */
	assert(!list_is_linked(&scheduler->current->queue_node));
	list_add(&scheduler->suspended_queue, &scheduler->current->queue_node);

	/* Since this is a non-preemptable cooperative scheduler and SVC exception has higher priority than the
	 * PENDSV exception (context switching) there is no need to lock the scheduler. Since we
	 * can only terminate the ourselves, clearing the current tcb and forcing a context switch
	 * will be enough to suspend the thread */
	scheduler->current = 0;
	request_context_switch();

	/* All ready to go */
	scheduler_enable_interrupts();
}
SCHEDULER_DECLARE_SVC(SCHEDULER_SUSPEND_SVC, scheduler_suspend_svc);

void scheduler_wait_svc(struct scheduler_frame *frame)
{
	unsigned long mask = frame->r0;
	unsigned long msecs = frame->r1;

	/* Assume every is ok */
	frame->r0 = 0;

	/* Protect the scheduler and tcb from interrupts */
	scheduler_disable_interrupts();

	/* Check to see if the event is all ready posted */
	if ((scheduler->current->events & mask) == 0) {

		/* Update the state and clear r0 in case we timeout */
		scheduler->current->state = TASK_BLOCKED;
		scheduler->current->psp->r0 = 0;

		/* Add to the suspended queue */
		assert(!list_is_linked(&scheduler->current->queue_node));
		list_add(&scheduler->suspended_queue, &scheduler->current->queue_node);

		/* Add a timeout if requested */
		if (msecs > 0)
			add_timer(&scheduler->current->timer, msecs, scheduler_wakeup, scheduler->current);

		/* Force a context switch */
		scheduler->current = 0;
		request_context_switch();
	}

	/* All ready to go */
	scheduler_enable_interrupts();
}
SCHEDULER_DECLARE_SVC(SCHEDULER_WAIT_SVC, scheduler_wait_svc);

void scheduler_join_svc(struct scheduler_frame *frame)
{
	unsigned long tid = frame->r0;
	unsigned long msecs = frame->r1;

	assert(tid != 0);

	/* Assume an error */
	frame->r0 = (uint32_t)-ESRCH;

	/* Hold off interrupts while move around */
	scheduler_disable_interrupts();

	/* We need to make sure the tcb is present in the scheduler */
	struct task_control_block *entry;
	list_for_each_entry(entry, &scheduler->tasks, scheduler_node)
		if (entry == (struct task_control_block *)tid) {

			/* If the thread is already dead, we are done return the task return code */
			if (entry->state != TASK_DEAD) {

				/* Nope we need to wait for the task. Since we are the currently scheduled task,
				 * update the tcb psp our scheduling frame which got pushed by the SVC
				 * handler linkage control. */
				scheduler->current->psp = frame;

				/* Update the state and */
				scheduler->current->state = TASK_BLOCKED;

				/* Is this a timed join, if so add a timer */
				if (msecs > 0)
					add_timer(&scheduler->current->timer, msecs, scheduler_wakeup, scheduler->current);

				/* Add ourselves to the join queue */
				list_add(&entry->join_queue, &scheduler->current->queue_node);

				/* Force a context switch */
				scheduler->current = 0;
				request_context_switch();
			}

			/* Found the cat */
			frame->r0 = 0;
			break;
		}

	/* All done the dogs chase the cat */
	scheduler_enable_interrupts();
}
SCHEDULER_DECLARE_SVC(SCHEDULER_JOIN_SVC, scheduler_join_svc);

static bool scheduler_is_viable(void)
{
	/* Check is easy one */
	if (!list_is_empty(&scheduler->ready_queue))
		return true;

	/* Nope, look through the suspend queue */
	struct task_control_block *potential_task;
	list_for_each_entry(potential_task, &scheduler->suspended_queue, queue_node)
		if (potential_task->state == TASK_SLEEPING || potential_task->state == TASK_BLOCKED)
			return true;

	/* Not viable, we should be cleanup and shutdown */
	return false;
}

struct scheduler_frame *scheduler_switch(struct scheduler_frame *current_frame)
{
	assert(scheduler != 0);

	/* Only push the current task if we have one */
	if (scheduler->current != 0) {

		/* Mark current task as ready, update the stack frame and push to back of fifo */
		scheduler->current->psp = current_frame;
		list_add(&scheduler->ready_queue, &scheduler->current->queue_node);
	}

	/* Try to get the next task */
	while ((scheduler->current = list_pop_entry(&scheduler->ready_queue, struct task_control_block, queue_node)) == 0) {

		/* If no potential tasks, terminate the scheduler */
		if (scheduler_is_viable()) {

			/* Enable interrupts, and wait for something to change */
			scheduler_enable_interrupts();

			/* Make sure interrupts are enable before entry wait mode */
			__ISB();

			/* Wait for an interrupt */
			__WFI();

			/* Need to protect queues again */
			scheduler_disable_interrupts();

		} else {

			/* No work possible, stop the scheduler disable the systick */
			SysTick->CTRL = 0;

			/* Move the initial frame pointer to the stack and clear in the scheduler support restarting */
			struct scheduler_frame *frame = scheduler->initial_frame;
			scheduler->initial_frame = 0;

			/* The syscall will return ok */
			frame->r0 = 0;

			/* This will return to the invoker of scheduler_start */
			return frame;
		}
	}

	/* Mark the task as running and return its scheduler frame */
	scheduler->current->state = TASK_RUNNING;
	return scheduler->current->psp;
}

unsigned long scheduler_create(const struct task_descriptor *descriptor)
{
	/* We must have a valid descriptor and stack */
	if (!scheduler || !descriptor || !descriptor->stack) {
		errno = EINVAL;
		return 0;
	}

	/* TODO Validate stack size is large enough */

	/* Initial the stack to dead beef for simple stack consumption measuements */
	memset(descriptor->stack, 0, descriptor->stack_size);

	/* Place the tcb at the bottom of the stack, double work aligned */
	struct task_control_block *tcb = (void *)(((uint32_t)descriptor->stack) + descriptor->stack_size - ALIGNMENT_ROUND(struct task_control_block, 8UL));

	/* Initialize the tcb and add to the scheduler task list */
	tcb->state = TASK_READY;
	tcb->task_descriptor = descriptor;
	tcb->events = 0;
	list_init(&tcb->join_queue);
	list_node_init(&tcb->timer.timer_node);
	list_node_init(&tcb->scheduler_node);
	list_node_init(&tcb->queue_node);
	list_push(&scheduler->tasks, &tcb->scheduler_node);

	/* Build the scheduler frame to use the PSP and run in privileged mode */
	tcb->psp = (void *)((uint32_t)tcb - ALIGNMENT_ROUND(struct scheduler_frame, 8UL));
/*	volatile size_t schedule_frame_size = sizeof(struct scheduler_frame); */
	struct scheduler_frame *task_frame = (struct scheduler_frame *)tcb->psp;
	task_frame->control = CONTROL_SPSEL_Msk;
	task_frame->pc = ((uint32_t)dispatch & ~0x01UL);
	task_frame->exec_return = 0xfffffffd;
	task_frame->psr = xPSR_T_Msk;
	task_frame->r0 = (uint32_t)tcb;
	task_frame->r1 = 0xdead0001;
	task_frame->r2 = 0xdead0002;
	task_frame->r3 = 0xdead0003;
	task_frame->r4 = 0xdead0004;
	task_frame->r5 = 0xdead0005;
	task_frame->r6 = 0xdead0006;
	task_frame->r7 = 0xdead0007;
	task_frame->r8 = 0xdead0008;
	task_frame->r9 = 0xdead0009;
	task_frame->r10 = 0xdead000a;
	task_frame->r11 = 0xdead000b;
	task_frame->r12 = 0xdead000c;

	task_frame->s0 = 0xfead0000;
	task_frame->s1 = 0xfead0001;
	task_frame->s2 = 0xfead0002;
	task_frame->s3 = 0xfead0003;
	task_frame->s4 = 0xfead0004;
	task_frame->s5 = 0xfead0005;
	task_frame->s6 = 0xfead0006;
	task_frame->s7 = 0xfead0007;
	task_frame->s8 = 0xfead0008;
	task_frame->s9 = 0xfead0009;
	task_frame->s10 = 0xfead000a;
	task_frame->s11 = 0xfead000b;
	task_frame->s12 = 0xfead000c;
	task_frame->s13 = 0xfead000d;
	task_frame->s14 = 0xfead000e;
	task_frame->s15 = 0xfead000f;
	task_frame->s16 = 0xfead0010;
	task_frame->s17 = 0xfead0011;
	task_frame->s18 = 0xfead0012;
	task_frame->s19 = 0xfead0013;
	task_frame->s20 = 0xfead0014;
	task_frame->s21 = 0xfead0015;
	task_frame->s22 = 0xfead0016;
	task_frame->s23 = 0xfead0017;
	task_frame->s24 = 0xfead0018;
	task_frame->s25 = 0xfead0019;
	task_frame->s26 = 0xfead001a;
	task_frame->s27 = 0xfead001b;
	task_frame->s28 = 0xfead001c;
	task_frame->s29 = 0xfead001d;
	task_frame->s30 = 0xfead001e;
	task_frame->s31 = 0xfead001f;


	/* Add the new task to the ready queue, locking not required as scheduler is not running yet */
	scheduler_disable_interrupts();
	list_push(&scheduler->ready_queue, &tcb->queue_node);
	scheduler_enable_interrupts();

	/* Since we pushed the tcb onto the ready queue, yield to allow the new task to run next */
	if (scheduler_is_running())
		scheduler_yield();

	/* All good */
	return (unsigned long)tcb;
}

int scheduler_run(struct scheduler *new_scheduler, int count, const struct task_descriptor *descriptors)
{
	/* Call should have provided the memory for the scheduler */
	if (scheduler || !new_scheduler) {
		errno = EINVAL;
		return -1;
	}

	/* Initialize the scheduler */
	new_scheduler->current = 0;
	new_scheduler->initial_frame = 0;
	new_scheduler->jiffies = 0;
	list_init(&new_scheduler->tasks);
	list_init(&new_scheduler->suspended_queue);
	list_init(&new_scheduler->ready_queue);
	list_init(&new_scheduler->timers);
	scheduler = new_scheduler;

	/* Create and initial tasks */
	for (int i = 0; i < count; ++i)
		if (scheduler_create(&descriptors[i]) == 0) {
			/* Flush any create tasks */
			list_init(&scheduler->ready_queue);
			return -1;
		}

	/* Enable the systick with 1ms granularity before changing the priorities because of brain-dead CMSIS priority configuration */
	SysTick_Config(SystemCoreClock / 1000UL);

	/* Initialize the new scheduler exception priorities */
	NVIC_SetPriorityGrouping(0);
	NVIC_SetPriority(PendSV_IRQn, SCHEDULER_PENDSV_PRIORITY);
	NVIC_SetPriority(SVCall_IRQn, SCHEDULER_SVC_PRIORITY);
	NVIC_SetPriority(SysTick_IRQn, SCHEDULER_SYSTICK_PRIORITY);

	/* Enable all interrupts */
	scheduler_enable_interrupts();

	/* We only return from this when the scheduler is shutdown */
	int result = svc_call0(SCHEDULER_START_SVC | SCHEDULER_SAVE_FRAME_LINKAGE_Msk | SCHEDULER_UPDATE_STACK_LINKAGE_Msk);
	if (result < 0) {
		errno = -result;
		return -1;
	}

	/* Clear the scheduler */
	scheduler = 0;

	/* All done */
	return 0;
}

int scheduler_is_running(void)
{
	/* Return invalid is there is not scheduler */
	if (!scheduler) {
		errno = EINVAL;
		return -1;
	}

	/* We are running is the initial frame has be set by the start service call */
	return scheduler->initial_frame != 0;
}

int scheduler_yield(void)
{
	/* We need a service call so that we appear to resume on return from this call */
	int result = svc_call0(SCHEDULER_YIELD_SVC);
	if (result < 0) {
		errno = -result;
		return -1;
	}

	/* All good */
	return 0;
}

unsigned long scheduler_get_tid(void)
{
	/* We are the current running task */
	return (unsigned int)scheduler->current;
}

int scheduler_suspend(void)
{
	/* We are suspending ourselves, then we need a scheduling frame from the service infrastructure */
	int result = svc_call1(SCHEDULER_SUSPEND_SVC | SCHEDULER_SAVE_FRAME_LINKAGE_Msk, 0);
	if (result < 0) {
		errno = -result;
		return -1;
	}

	/* All good */
	return 0;
}

int scheduler_sleep(unsigned long msecs)
{
	/* Jiffies should be greater 0, just yield otherwise */
	if (msecs == 0)
		return scheduler_yield();

	/* We are timed suspending ourselves, then we need a scheduling frame from the service infrastructure */
	int result = svc_call1(SCHEDULER_SUSPEND_SVC | SCHEDULER_SAVE_FRAME_LINKAGE_Msk, msecs);
	if (result < 0) {
		errno = -result;
		return -1;
	}

	/* All good */
	return 0;
}

int scheduler_resume(unsigned long tid)
{
	struct task_control_block *tcb = (struct task_control_block *)tid;
	struct task_control_block *entry;

	/* SysTick might collide this resume */
	scheduler_disable_interrupts();

	/* Look the tcb in the suspended queue */
	list_for_each_entry(entry, &scheduler->suspended_queue, queue_node)

		/* Did find it? */
		if (entry == tcb) {

			/* Remove it */
			list_remove(&entry->queue_node);

			/* Might have an associated timer, remove */
			delete_timer(&tcb->timer);

			/* Clear the return so we can indicate no timeout */
			entry->psp->r0 = 0;

			/* Mark as ready */
			entry->state = TASK_READY;

			/* Push to the front of the ready, so it runs next */
			list_push(&scheduler->ready_queue, &entry->queue_node);

			/* Let the SysTick and others run */
			scheduler_enable_interrupts();

			/* A thread calling resume implies a context switch, but sinces this is a non-preemptable scheduler, interrupt context caller do not */
			if (!is_interrupt_context())
				scheduler_yield();

			/* All done */
			return 0;
		}

	/* Well, no joy, enable interrupts and return an error */
	scheduler_enable_interrupts();
	errno = ESRCH;
	return -1;
}

unsigned long scheduler_wait_event(unsigned long mask, unsigned long msecs)
{
	unsigned long masked_events = 0;

	/* Protect the event value from interrupts */
	scheduler_disable_interrupts();

	/* Check to see if we have the event we are looking for */
	while ((scheduler->current->events & mask) == 0) {

		/* Enable interrupts */
		scheduler_enable_interrupts();

		/* Wait for a notification and check for timeout */
		int result = svc_call2(SCHEDULER_WAIT_SVC | SCHEDULER_SAVE_FRAME_LINKAGE_Msk, mask, msecs);
		if (result < 0) {
			errno = -result;
			return 0;
		}

		/* Disable again */
		scheduler_disable_interrupts();
	}

	/* Save and clear masked events */
	masked_events = scheduler->current->events & mask;
	scheduler->current->events &= ~mask;

	/* Let everyone go now */
	scheduler_enable_interrupts();

	/* All good, we matched */
	return masked_events;
}

int scheduler_notify_event(unsigned long tid, unsigned long events)
{
	struct task_control_block *tcb = (struct task_control_block *)tid;

	assert(tcb != 0);

	/* Protect the event value, ok in interrupts */
	scheduler_disable_interrupts();

	/* OR in the notifications */
	tcb->events |= events;

	/* Done with updates */
	scheduler_enable_interrupts();

	/* Try to resume the thread, this works in both handler and thread modes who will check for unblocking */
	return scheduler_resume(tid);
}

int scheduler_join(unsigned long tid, int *return_code, unsigned long msecs)
{
	/* Block until the thread is dead */
	int result = svc_call2(SCHEDULER_JOIN_SVC | SCHEDULER_SAVE_FRAME_LINKAGE_Msk, tid, msecs);
	if (result < 0) {
		errno = -result;
		return -1;
	}

	/* Protect the scheduler */
	scheduler_disable_interrupts();

	/* If we are the last joiner we need to clean up */
	struct task_control_block *tcb = (struct task_control_block *)tid;
	if (list_is_empty(&tcb->join_queue))
		list_remove(&tcb->scheduler_node);

	/* Done with the scheduler */
	scheduler_enable_interrupts();

	/* Save the return code */
	if (return_code)
		*return_code = tcb->return_code;

	/* All done */
	return 0;
}

