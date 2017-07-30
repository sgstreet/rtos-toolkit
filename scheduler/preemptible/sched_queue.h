/*
 * Copyright (C) 2017 Red Rocket Computing, LLC
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * scheduler.h
 *
 * Created on: July 28, 2017
 *     Author: Stephen Street (stephen@redrocketcomputing.com)
 */

#ifndef SCHED_QUEUE_H_
#define SCHED_QUEUE_H_

#include <preempt-sched.h>

struct scheduler_queue
{
	unsigned long bucket_mask;
	struct linked_list buckets[SCHEDULER_NUM_TASK_PRIORITIES];
};

static inline void scheduler_queue_init(struct scheduler_queue *queue)
{
	queue->bucket_mask = 0;

	for (int i = 0; i < SCHEDULER_NUM_TASK_PRIORITIES; ++i)
		list_init(&queue->buckets[i]);
}

static inline void scheduler_queue_push(struct scheduler_queue *queue, struct task_control_block *tcb)
{
	list_add(queue->buckets[tcb->current_priority], tcb->queue_node);
}

static inline struct task_control_block *scheduler_queue_pop(struct scheduler_queue *queue)
{
	return queue->bucket_mask != 0 ? list_pop(queue->buckets[__builtin_clzl(queue->bucket_mask)]) : 0;
}

#endif /* SCHED_QUEUE_H_ */
