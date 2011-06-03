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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#include "timeout.h"

/** Queue Create
 * @return pointer to new queue
 */
struct queue *
queue_create ()
{
    struct queue *queue;

    if ((queue = malloc (sizeof (*queue))) == NULL)
        return NULL;

    queue->size = 0;
    queue->head = NULL;
    queue->tail = NULL;

    pthread_mutex_init (&queue->lock, NULL);

    return queue;
}

/** Queue Start
 * @return 0 on success, -1 on failure
 */
int
queue_start (struct queue *queue)
{
    if (pthread_create (&queue->thread, NULL, timeout_thread, (void *)queue))
        return -1;

    return 0;
}

/** Queue Stop
 * @return 0 on success, -1 on failure
 */
int
queue_stop (struct queue *queue)
{
    if (pthread_cancel (queue->thread))
        return -1;

    return 0;
}

/** Queue Element Create
 * @return pointer to a new queue element
 */
struct queue_element *
queue_element_create (const void *p_key, unsigned int i_key_size)
{
    struct queue_element *elem;

    if ((elem = malloc (sizeof (*elem))) == NULL)
        return NULL;

    if ((elem->key = malloc (i_key_size)) == NULL)
    {
        free (elem);
        return NULL;
    }
    memcpy (elem->key, p_key, i_key_size);

    elem->prev = NULL;
    elem->next = NULL;
    gettimeofday (&elem->time, NULL);

    return elem;
}

/** Queue Destroy
 * @return -1 on failure, 0 on success
 */
int
queue_destroy (struct queue *queue)
{
    if (queue == NULL)
        return -1;

    while (queue->size > 0)
        queue_delete (queue, queue->head);

    free (queue);

    return 0;
}

/** Queue Pop
 * Remove the element from the list
 * @return -1 on failure, 0 on success
 */
int
queue_pop (struct queue *queue, struct queue_element *elem)
{
    if (queue == NULL || queue->size == 0 || elem == NULL)
        return -1;


    if (elem == queue->head)
    {
        queue->head = elem->next;

        if (queue->head == NULL)
            queue->tail = NULL;
        else
            queue->head->prev = NULL;
    }
    else
    {
        elem->prev->next = elem->next;

        if (elem->next == NULL)
            queue->tail = elem->prev;
        else
            elem->next->prev = elem->prev;
    }

    elem->prev = NULL;
    elem->next = NULL;

    queue->size--;
    return 0;
}

/** Queue Delete
 * Delete an element from the list
 * @return -1 on failure, 0 on success
 */
int
queue_delete (struct queue *queue, struct queue_element *elem)
{
    if (queue_pop (queue, elem))
        return -1;

    if (queue->destroy != NULL)
        queue->destroy (elem->key);

    free (elem->key);
    free (elem);

    return 0;
}

/** Queue Element Insert
 * @return -1 on failure, 0 on success
 */
int
queue_insert (struct queue *queue, struct queue_element *elem)
{
    if (queue == NULL || elem == NULL)
        return -1;

    pthread_mutex_lock (&queue->lock);

    if (queue->size == 0)
    {
        queue->head = elem;
        queue->tail = elem;
        queue->head->prev = NULL;
        queue->head->next = NULL;
    }
    else
    {
        elem->next = NULL;
        elem->prev = queue->tail;
        queue->tail->next = elem;
        queue->tail = elem;
    }

    queue->size++;
    gettimeofday (&elem->time, NULL);

    pthread_mutex_unlock (&queue->lock);

    return 0;
}

/** Update the atime on the element and bump it up the queue
 * @return -1 on failure, 0 on success 
 */
int
queue_bump (struct queue *queue, struct queue_element *elem)
{
    if (queue == NULL || elem == NULL)
        return -1;

    if (queue_pop (queue, elem))
        return -1;

    queue_insert (queue, elem);

    return 0;
}

/** Queue Find
 * @return pointer to matching element
 */
struct queue_element *
queue_find (struct queue *queue, const void *p_key)
{
    struct queue_element *it;

    for (it = queue->head; it; it = it->next)
        if (queue->compare (p_key, it->key) == 0)
            return it;

    return NULL;
}

/** Timeout Thread
 * @return void *
 */
void *
timeout_thread (void *args)
{
    struct queue *queue = (struct queue *)args;
    struct timespec timeout;
    timeout.tv_sec = 1;
    timeout.tv_nsec = 0;

    while (1)
    {
        nanosleep(&timeout, NULL);
        queue_timeout (queue);
    }
}

/** Timeout old elements in the queue 
 * @return number of elements timed out
 */
int
queue_timeout (struct queue *queue)
{
    struct queue_element *it;
    struct timeval timeout;
    int removed = 0;

    gettimeofday (&timeout, NULL);
    timeout.tv_sec -= QUEUE_TIMEOUT;

    for (it = queue->tail; it; it = queue->tail)
    {
        if (it->time.tv_sec > timeout.tv_sec)
            break;

        queue_delete (queue, it);
        removed++;
    }

    printf ("removed (%d) elements\n", removed);
    return removed;
}

/** Print a given element
 * @return void
 */
void
queue_element_print (struct queue_element *elem)
{
    char s[50];
    strftime (s, sizeof (s), "%b %e, %Y %T",
              localtime ((const time_t *) &elem->time));

    printf ("    prev: %p\n", elem->prev);
    printf ("    next: %p\n", elem->next);
    printf ("    time: %s\n", s);
    printf ("     key: %p\n", elem->key);
}

/** Print all the elements in the queue 
 * @return void
 */
void
queue_print (struct queue *queue)
{
    struct queue_element *it;

    printf ("===================================================\n");
    printf ("Queue Size: %d\n", queue->size);
    printf ("Queue head: %p\n", queue->head);
    printf ("Queue tail: %p\n\n", queue->tail);

    for (it = queue->head; it; it = it->next)
        queue_element_print (it);
}
