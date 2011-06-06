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

/** Default Queue Timeout value
 */
#define SCHEDULER_TIMEOUT 3

/** Expire interval 
 * how often to check for expired elements
 */
#define EXPIRE_INTERVAL 1

/** State of the sched
 */
typedef enum
{
    SCHEDULER_STOPPED,
    SCHEDULER_STARTED
} SCHEDULER_STATE;

/** Schedule Queue Element
 */
struct sched_element
{
    struct sched_element *prev;
    struct sched_element *next;
    struct timeval time;        // access time
    void *key;
};

/** Schedule Queue Structure
 */
struct sched
{
    struct sched_element *head;
    struct sched_element *tail;
    int timeout;
    int size;

    pthread_t thread;
    pthread_mutex_t lock;
    SCHEDULER_STATE state;

    int (*compare) (const void *p1, const void *p2);
    int (*task) (const void *p);
};

/** Timer routine
 */
extern void *schedule_runner (void *args);

/** Create a new sched 
 */
extern struct sched *sched_create ();

/** Start the expiration thread for the sched
 */
int sched_start (struct sched *sched);

/** Stop the expiration thread fro the sched
 */
int sched_stop (struct sched *sched);

/** Create a new sched element
 */
extern struct sched_element *sched_element_create (const void *p_key,
                                                   unsigned int i_key_size);

/** Destroy a sched 
 */
extern int sched_destroy (struct sched *sched);

/** Pop an element out of the sched
 */
extern int sched_pop (struct sched *sched, struct sched_element *elem);

/** Delete an element from the sched
 */
extern int sched_delete (struct sched *sched, struct sched_element *elem);

/** Insert an element into the sched
 */
extern int sched_insert (struct sched *sched, struct sched_element *elem);

/** Push element to top of sched and update access time
 */
extern int sched_bump (struct sched *sched, struct sched_element *elem);

/** Find an element that matches the data
 */
extern struct sched_element *sched_find (struct sched *sched,
                                         const void *p_key);

/** Timeout old elements in the sched
 */
extern int sched_timeout (struct sched *sched);

#endif /* __TIMEOUT_SCHEDULER_H__ */
