/*
 * Copyright(c) 2012 Tim Ruehsen
 *
 * This file is part of libmget.
 *
 * Libmget is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Libmget is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libmget.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * Double linked list routines
 *
 * Changelog
 * 25.04.2012  Tim Ruehsen  created
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdlib.h>
#include <string.h>

#include <libmget.h>

#include <xalloc.h>

typedef struct MGET_LISTNODE MGET_LISTNODE;

struct MGET_LISTNODE {
	MGET_LISTNODE
		*next,
		*prev;
};

void *mget_list_append(MGET_LISTNODE **list, const void *elem, size_t size)
{
	// allocate space for node and data in one row
	MGET_LISTNODE *node = xmalloc(sizeof(MGET_LISTNODE) + size);

	memcpy(node + 1, elem, size);

	if (!*list) {
		// <*list> is an empty list
		*list = node;
		node->next = node->prev = node;
	} else {
		node->next = *list;
		node->prev = (*list)->prev;
		(*list)->prev->next = node;
		(*list)->prev = node;
	}

	return node + 1;
}

void *mget_list_prepend(MGET_LISTNODE **list, const void *elem, size_t size)
{
	if (!*list) {
		return mget_list_append(list, elem, size);
	} else {
		return mget_list_append(&(*list)->prev, elem, size);
	}
}

void mget_list_remove(MGET_LISTNODE **list, void *elem)
{
	MGET_LISTNODE *node = ((MGET_LISTNODE *)elem) - 1;

	if (node->prev == node->next && node == node->prev) {
		// last node in list
		if (list && *list && node == *list)
			*list = NULL;
	} else {
		node->prev->next = node->next;
		node->next->prev = node->prev;
		if (list && *list && node == *list)
			*list = node->next;
	}
	xfree(node);
}

void *mget_list_getfirst(const MGET_LISTNODE *list)
{
	return (void *)(list + 1);
}

void *mget_list_getlast(const MGET_LISTNODE *list)
{
	return (void *)(list->prev + 1);
}

int mget_list_browse(const MGET_LIST *list, int (*browse)(void *context, void *elem), void *context)
{
	int ret = 0;

	if (list) {
		const MGET_LISTNODE *end = list->prev, *cur = list;

		while ((ret = browse(context, (void *)(cur + 1))) == 0 && cur != end)
			cur = cur->next;
	}

	return ret;
}

void mget_list_free(MGET_LIST **list)
{
	while (*list)
		mget_list_remove(list, ((MGET_LISTNODE *) * list) + 1);
}

/*
void mget_list_dump(const MGET_LIST *list)
{
	if (list) {
		const MGET_LISTNODE *cur = list;

		do {
			log_printf("%p: next %p prev %p\n", cur, cur->next, cur->prev);
			cur = cur->next;
		} while (cur != list);
	} else
		log_printf("empty\n");
}
*/
