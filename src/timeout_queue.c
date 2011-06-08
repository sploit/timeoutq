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

#include "timeout_queue.h"

/** Schedule Queue Create
 * @return pointer to new tmq
 */
struct tmq *
tmq_create ()
{
    struct tmq *tmq;

    if ((tmq = malloc (sizeof (*tmq))) == NULL)
        return NULL;

    tmq->size = 0;
    tmq->head = NULL;
    tmq->tail = NULL;

    pthread_mutex_init (&tmq->lock, NULL);

    tmq->state = SCHEDULER_STOPPED;

    return tmq;
}

/** Start the clock 
 * @return 0 on success, -1 on failure
 */
int
tmq_start (struct tmq *tmq)
{
    if (tmq->state != SCHEDULER_STOPPED)
        return -1;

    if (pthread_create (&tmq->thread, NULL, tmq_thread, (void *)tmq))
        return -1;

    tmq->state = SCHEDULER_STARTED;

    return 0;
}

/** Schedule Queue Stop
 * @return 0 on success, -1 on failure
 */
int
tmq_stop (struct tmq *tmq)
{
    if (tmq->state != SCHEDULER_STARTED)
        return -1;

    tmq->state = SCHEDULER_STOPPED;

    if (pthread_cancel (tmq->thread))
        return -1;
/*
    if (pthread_join (tmq->thread, NULL))
        return -1;
*/

    return 0;
}

/** Schedule Queue Element Create
 * @return pointer to a new tmq element
 */
struct tmq_element *
tmq_element_create (const void *p_key, unsigned int i_key_size)
{
    struct tmq_element *elem;

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

/** Schedule Queue Destroy
 * @return -1 on failure, 0 on success
 */
int
tmq_destroy (struct tmq *tmq)
{
    if (tmq == NULL)
        return -1;

    if (tmq->state == SCHEDULER_STARTED)
        return -1;

    while (tmq->size > 0)
        tmq_delete (tmq, tmq->head);

    free (tmq);

    return 0;
}

/** Schedule Queue Pop
 * Remove the element from the list
 * @return -1 on failure, 0 on success
 */
int
tmq_pop (struct tmq *tmq, struct tmq_element *elem)
{
    if (tmq == NULL || tmq->size == 0 || elem == NULL)
        return -1;


    if (elem == tmq->head)
    {
        tmq->head = elem->next;

        if (tmq->head == NULL)
            tmq->tail = NULL;
        else
            tmq->head->prev = NULL;
    }
    else
    {
        elem->prev->next = elem->next;

        if (elem->next == NULL)
            tmq->tail = elem->prev;
        else
            elem->next->prev = elem->prev;
    }

    elem->prev = NULL;
    elem->next = NULL;

    tmq->size--;
    return 0;
}

/** Schedule Queue Delete
 * Delete an element from the list
 * @return -1 on failure, 0 on success
 */
int
tmq_delete (struct tmq *tmq, struct tmq_element *elem)
{
    if (tmq == NULL || elem == NULL || elem->key == NULL)
        return -1;

    if (tmq_pop (tmq, elem))
        return -1;

    free (elem->key);
    free (elem);

    return 0;
}

/** Schedule Queue Element Insert
 * @return -1 on failure, 0 on success
 */
int
tmq_insert (struct tmq *tmq, struct tmq_element *elem)
{
    if (tmq == NULL || elem == NULL)
        return -1;

    pthread_mutex_lock (&tmq->lock);

    if (tmq->size == 0)
    {
        tmq->head = elem;
        tmq->tail = elem;
        tmq->head->prev = NULL;
        tmq->head->next = NULL;
    }
    else
    {
        elem->next = NULL;
        elem->prev = tmq->tail;
        tmq->tail->next = elem;
        tmq->tail = elem;
    }

    tmq->size++;
    gettimeofday (&elem->time, NULL);

    pthread_mutex_unlock (&tmq->lock);

    return 0;
}

/** Update the atime on the element and bump it up the tmq
 * @return -1 on failure, 0 on success 
 */
int
tmq_bump (struct tmq *tmq, struct tmq_element *elem)
{
    if (tmq == NULL || elem == NULL)
        return -1;

    if (tmq_pop (tmq, elem))
        return -1;

    tmq_insert (tmq, elem);

    return 0;
}

/** Schedule Queue Find
 * @return pointer to matching element
 */
struct tmq_element *
tmq_find (struct tmq *tmq, const void *p_key)
{
    struct tmq_element *it;

    for (it = tmq->head; it; it = it->next)
        if (tmq->compare (p_key, it->key) == 0)
            return it;

    return NULL;
}

/** Timeout old elements in the tmq 
 * @return number of elements timed out
 */
int
tmq_timeout (struct tmq *tmq)
{
    struct tmq_element *it;
    struct timeval delta;
    int removed = 0;

    gettimeofday (&delta, NULL);
    delta.tv_sec -= DEFAULT_TIME; 

    for (it = tmq->tail; it; it = tmq->tail)
    {
        if (it->time.tv_sec > delta.tv_sec)
            break;

        if (tmq->task != NULL)
            tmq->task (it->key);

        tmq_delete (tmq, it);
        removed++;
    }

    printf ("Removed (%d) elements\n", removed);
    return removed;
}

/** Timeout Thread
 * @return void *
 */
void *
tmq_thread (void *args)
{
    struct tmq *tmq = (struct tmq *)args;
    struct timespec schedule;
    schedule.tv_sec = SCHEDULE_INTERVAL;
    schedule.tv_nsec = 0;

    pthread_setcancelstate (PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype (PTHREAD_CANCEL_DEFERRED, NULL);

    while (1)
    {
        nanosleep(&schedule, NULL);
        tmq_timeout (tmq);
    }

    return (void *)0;
}
