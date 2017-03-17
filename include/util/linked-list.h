/*
 * Copyright (C) 2017 Red Rocket Computing, LLC
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * linked-list.h
 *
 * Created on: Feb 7, 2017
 *     Author: Stephen Street (stephen@redrocketcomputing.com)
 */

#ifndef LIST_H_
#define LIST_H_

#include <stdbool.h>
#include <util/container_of.h>

struct list_node
{
	struct list_node *next;
	struct list_node *prev;
};

struct linked_list
{
	struct list_node head;
};

#define LIST_INIT(list) {{&list.head, &list.head}}
#define LIST_NODE_INIT(node) {{&node, &node}}

static inline void list_init(struct linked_list *list)
{
	list->head.next = &list->head;
	list->head.prev = &list->head;
}

static inline void list_node_init(struct list_node *node)
{
	node->next = node;
	node->prev = node;
}

static inline bool list_is_empty(const struct linked_list *list)
{
	return list->head.next == &list->head;
}

static inline void list_insert_after(struct list_node *first, struct list_node *second)
{
	second->next = first->next;
	second->prev = first;
	first->next->prev = second;
	first->next = second;
}

static inline void list_insert_before(struct list_node *first, struct list_node *second)
{
	second->next = first;
	second->prev = first->prev;
	first->prev->next = second;
	first->prev = second;
}

static inline void list_remove(struct list_node *node)
{
	node->next->prev = node->prev;
	node->prev->next = node->next;
	list_node_init(node);
}

static inline struct list_node *list_front(const struct linked_list *list)
{
	if (list_is_empty(list))
		return NULL;
	return list->head.next;
}

static inline struct list_node *list_back(const struct linked_list *list)
{
	if (list_is_empty(list))
		return NULL;
	return list->head.prev;
}

static inline void list_add(struct linked_list *list, struct list_node *node)
{
	node->next = &list->head;
	node->prev = list->head.prev;
	list->head.prev->next = node;
	list->head.prev = node;
}

static inline void list_push(struct linked_list *list, struct list_node *node)
{
	node->next = list->head.next;
	node->prev = &list->head;
	list->head.next->prev = node;
	list->head.next = node;
}

static inline struct list_node *list_pop(struct linked_list *list)
{
	struct list_node *front = NULL;
	if (!list_is_empty(list))
	{
		front = list->head.next;
		list_remove(front);
	}
	return front;
}

static inline struct list_node *list_next(const struct linked_list *list, struct list_node *node)
{
	if (node->next == &list->head)
		return NULL;
	return node->next;
}

static inline struct list_node *list_prev(const struct linked_list *list, struct list_node *node)
{
	if (node->prev == &list->head)
		return NULL;
	return node->prev;
}

static inline int list_is_linked(struct list_node *node)
{
	return node->next != node;
}

#define list_pop_entry(list, type, member) container_of_or_null(list_pop(list), type, member)

#define list_entry(ptr, type, member) container_of_or_null(ptr, type, member)
#define list_first_entry(list, type, member) list_entry((list)->head.next, type, member)
#define list_last_entry(list, type, member) list_entry((list)->head.prev, type, member)
#define list_next_entry(position, member) list_entry((position)->member.next, typeof(*(position)), member)

#define list_for_each(cursor, list) \
	for (cursor = (list)->head.next; cursor != &(list)->head; cursor = cursor->next)

#define list_for_each_mutable(cursor, current, list) \
	for (cursor = (list)->head.next, current = cursor->next; cursor != &(list)->head; cursor = current, current = cursor->next)

#define list_for_each_entry(cursor, list, member) \
	for (cursor = list_first_entry(list, typeof(*cursor), member); &cursor->member != &(list)->head; cursor = list_next_entry(cursor, member))

#define list_for_each_entry_mutable(cursor, current, list, member) \
	for (cursor = list_first_entry(list, typeof(*cursor), member), current = list_next_entry(cursor, member); &cursor->member != &(list)->head; cursor = current, current = list_next_entry(current, member))

#endif
