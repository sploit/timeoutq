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

#include "schedule.h"

/** Schedule Queue Create
 * @return pointer to new sched
 */
struct sched *
sched_create ()
{
    struct sched *sched;

    if ((sched = malloc (sizeof (*sched))) == NULL)
        return NULL;

    sched->size = 0;
    sched->head = NULL;
    sched->tail = NULL;

    pthread_mutex_init (&sched->lock, NULL);

    sched->state = SCHEDULER_STOPPED;

    return sched;
}

/** Schedule Queue Start
 * @return 0 on success, -1 on failure
 */
int
sched_start (struct sched *sched)
{
    if (sched->state != SCHEDULER_STOPPED)
        return -1;

    if (pthread_create (&sched->thread, NULL, schedule_runner, (void *)sched))
        return -1;

    sched->state = SCHEDULER_STARTED;

    return 0;
}

/** Schedule Queue Stop
 * @return 0 on success, -1 on failure
 */
int
sched_stop (struct sched *sched)
{
    if (sched->state != SCHEDULER_STARTED)
        return -1;

    sched->state = SCHEDULER_STOPPED;

    if (pthread_cancel (sched->thread))
        return -1;
/*
    if (pthread_join (sched->thread, NULL))
        return -1;
*/

    return 0;
}

/** Schedule Queue Element Create
 * @return pointer to a new sched element
 */
struct sched_element *
sched_element_create (const void *p_key, unsigned int i_key_size)
{
    struct sched_element *elem;

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
sched_destroy (struct sched *sched)
{
    if (sched == NULL)
        return -1;

    if (sched->state == SCHEDULER_STARTED)
        return -1;

    while (sched->size > 0)
        sched_delete (sched, sched->head);

    free (sched);

    return 0;
}

/** Schedule Queue Pop
 * Remove the element from the list
 * @return -1 on failure, 0 on success
 */
int
sched_pop (struct sched *sched, struct sched_element *elem)
{
    if (sched == NULL || sched->size == 0 || elem == NULL)
        return -1;


    if (elem == sched->head)
    {
        sched->head = elem->next;

        if (sched->head == NULL)
            sched->tail = NULL;
        else
            sched->head->prev = NULL;
    }
    else
    {
        elem->prev->next = elem->next;

        if (elem->next == NULL)
            sched->tail = elem->prev;
        else
            elem->next->prev = elem->prev;
    }

    elem->prev = NULL;
    elem->next = NULL;

    sched->size--;
    return 0;
}

/** Schedule Queue Delete
 * Delete an element from the list
 * @return -1 on failure, 0 on success
 */
int
sched_delete (struct sched *sched, struct sched_element *elem)
{
    if (sched == NULL || elem == NULL || elem->key == NULL)
        return -1;

    if (sched_pop (sched, elem))
        return -1;

    free (elem->key);
    free (elem);

    return 0;
}

/** Schedule Queue Element Insert
 * @return -1 on failure, 0 on success
 */
int
sched_insert (struct sched *sched, struct sched_element *elem)
{
    if (sched == NULL || elem == NULL)
        return -1;

    pthread_mutex_lock (&sched->lock);

    if (sched->size == 0)
    {
        sched->head = elem;
        sched->tail = elem;
        sched->head->prev = NULL;
        sched->head->next = NULL;
    }
    else
    {
        elem->next = NULL;
        elem->prev = sched->tail;
        sched->tail->next = elem;
        sched->tail = elem;
    }

    sched->size++;
    gettimeofday (&elem->time, NULL);

    pthread_mutex_unlock (&sched->lock);

    return 0;
}

/** Update the atime on the element and bump it up the sched
 * @return -1 on failure, 0 on success 
 */
int
sched_bump (struct sched *sched, struct sched_element *elem)
{
    if (sched == NULL || elem == NULL)
        return -1;

    if (sched_pop (sched, elem))
        return -1;

    sched_insert (sched, elem);

    return 0;
}

/** Schedule Queue Find
 * @return pointer to matching element
 */
struct sched_element *
sched_find (struct sched *sched, const void *p_key)
{
    struct sched_element *it;

    for (it = sched->head; it; it = it->next)
        if (sched->compare (p_key, it->key) == 0)
            return it;

    return NULL;
}

/** Timeout old elements in the sched 
 * @return number of elements timed out
 */
int
sched_timeout (struct sched *sched)
{
    struct sched_element *it;
    struct timeval timeout;
    int removed = 0;

    gettimeofday (&timeout, NULL);
    timeout.tv_sec -= SCHEDULER_TIMEOUT;

    for (it = sched->tail; it; it = sched->tail)
    {
        if (it->time.tv_sec > timeout.tv_sec)
            break;

        if (sched->task != NULL)
            sched->task (it->key);

        sched_delete (sched, it);
        removed++;
    }

    printf ("Removed (%d) elements\n", removed);
    return removed;
}

/** Timeout Thread
 * @return void *
 */
void *
schedule_runner (void *args)
{
    struct sched *sched = (struct sched *)args;
    struct timespec timeout;
    timeout.tv_sec = EXPIRE_INTERVAL;
    timeout.tv_nsec = 0;

    pthread_setcancelstate (PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype (PTHREAD_CANCEL_DEFERRED, NULL);

    while (1)
    {
        nanosleep(&timeout, NULL);
        sched_timeout (sched);
    }

    return (void *)0;
}
