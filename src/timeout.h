/******************************************************************************
 * Copyright (C) 2011 Victor J. Roemer
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *****************************************************************************/
#ifndef __TIMEOUT_QUEUE_H__
#define __TIMEOUT_QUEUE_H__

#include <sys/time.h>

/** Default Queue Timeout value
 */
#define QUEUE_TIMEOUT 180

/** Queue Element
 */
struct queue_element
{
    struct queue_element *prev;
    struct queue_element *next;
    struct timeval time;        // access time
    void *key;
};

/** Queue Structure
 */
struct queue
{
    struct queue_element *head;
    struct queue_element *tail;
    int timeout;
    int size;

    int (*compare) (const void *p1, const void *p2);
    int (*destroy) (const void *p);
};

/** Create a new queue 
 */
extern struct queue *queue_create ();

/** Create a new queue element
 */
extern struct queue_element *queue_element_create (const void *p_key,
                                                   unsigned int i_key_size);

/** Destroy a queue 
 *
 * XXX: this will queue->destroy all of your elements, meaning if your
 * destroyer function removes the element from a hash table it will remove
 * all of them.
 *
 * TODO: Fix the above
 */
extern int queue_destroy (struct queue *queue);

/** Pop an element out of the queue
 */
extern int queue_pop (struct queue *queue, struct queue_element *elem);

/** Delete an element from the queue
 */
extern int queue_delete (struct queue *queue, struct queue_element *elem);

/** Insert an element into the queue
 */
extern int queue_insert (struct queue *queue, struct queue_element *elem);

/** Push element to top of queue and update access time
 */
extern int queue_bump (struct queue *queue, struct queue_element *elem);

/** Find an element that matches the data
 */
extern struct queue_element *queue_find (struct queue *queue,
                                         const void *p_key);

/** Timeout old elements in the queue
 */
extern int queue_timeout (struct queue *queue);

/** Dump queue element to screen
 */
extern void queue_element_print (struct queue_element *elem);

/** Dump queue to screen
 */
extern void queue_print (struct queue *queue);

#endif /* __TIMEOUT_QUEUE_H__ */
