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
#ifndef __TIMEOUT_SCHEDULER_H__
#define __TIMEOUT_SCHEDULER_H__

#include <sys/time.h>
#include <pthread.h>

/** Default Queue Time value
 */
#define DEFAULT_TIME 180

/** Schedule check interval 
 * how often to check for expired elements
 */
#define SCHEDULE_INTERVAL 1

/** State of the tmq
 */
typedef enum
{
    SCHEDULER_STOPPED,
    SCHEDULER_STARTED
} SCHEDULER_STATE;

/** Schedule Queue Element
 */
struct tmq_element
{
    struct tmq_element *prev;
    struct tmq_element *next;
    struct timeval time;        /* access time */
    void *key;
};

/** Schedule Queue Structure
 */
struct tmq
{
    struct tmq_element *head;
    struct tmq_element *tail;
    int tmq;
    int size;

    pthread_t thread;
    pthread_mutex_t lock;
    SCHEDULER_STATE state;

    int (*compare) (const void *p1, const void *p2);
    int (*task) (const void *p);
};

/** Timer routine
 */
extern void *tmq_thread (void *args);

/** Create a new tmq 
 */
extern struct tmq *tmq_create ();

/** Start the expiration thread for the tmq
 */
int tmq_start (struct tmq *tmq);

/** Stop the expiration thread fro the tmq
 */
int tmq_stop (struct tmq *tmq);

/** Create a new tmq element
 */
extern struct tmq_element *tmq_element_create (const void *p_key,
                                                   unsigned int i_key_size);

/** Destroy a tmq 
 */
extern int tmq_destroy (struct tmq *tmq);

/** Pop an element out of the tmq
 */
extern int tmq_pop (struct tmq *tmq, struct tmq_element *elem);

/** Delete an element from the tmq
 */
extern int tmq_delete (struct tmq *tmq, struct tmq_element *elem);

/** Insert an element into the tmq
 */
extern int tmq_insert (struct tmq *tmq, struct tmq_element *elem);

/** Push element to top of tmq and update access time
 */
extern int tmq_bump (struct tmq *tmq, struct tmq_element *elem);

/** Find an element that matches the data
 */
extern struct tmq_element *tmq_find (struct tmq *tmq,
                                         const void *p_key);

/** Timeout old elements in the tmq
 */
extern int tmq_timeout (struct tmq *tmq);

#endif /* __TIMEOUT_SCHEDULER_H__ */
