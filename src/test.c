/* Some test code for my expiration thread magic
 * Copyright (c) 2011, Victor J Roemer
 *
 * $ gcc -pthread -o test *.c
 * $ ./test
 * removed (0) elements
 * removed (0) elements
 * removed (0) elements
 * removed (0) elements
 * removed (1000) elements
 * removed (0) elements
 * removed (0) elements
 * removed (1000) elements
 * removed (0) elements
 * removed (0) elements
 * removed (1000) elements
 * removed (0) elements
 * removed (0) elements
 * removed (1000) elements
 * removed (0) elements
 * removed (0) elements
 * removed (1000) elements
 * removed (0) elements
 * removed (0) elements
 * removed (1000) elements
 * removed (0) elements
 * removed (0) elements
 * removed (1000) elements
 * removed (0) elements
 * removed (0) elements
 * removed (1000) elements
 * removed (0) elements
 * removed (0) elements
 * removed (1000) elements
 */
#include "schedule.h"

int task () { return 0; };

int main (int argc, char *argv[])
{
    struct sched *sched = sched_create ();
    struct sched_element *elem;
    int key;

    /* for nanosleep */
    struct timespec time;
    time.tv_sec = 3;
    time.tv_nsec = 0;

    /* Set the destroyer
     * TODO rename destroy to timeout_callback or something as it is the
     * action that will be taken once an element has expired */
    sched->task = task;

    /* Start the expiration thread */
    sched_start (sched);

    for (key = 0; key < 3000; key++)
    {
        /* Space time insertions out a bit */
        if ((key % 1000) == 0)
            nanosleep (&time, NULL);

        elem = sched_element_create (&key, sizeof(key));
        sched_insert (sched, elem);
    }

    /* the timeout thread will ensure that we eventually 
     * remove all the elements from the sched */
    while (sched->size > 0);

    /* kill off the expiration thread */
    sched_stop (sched);

    /* Queue destroy removes all elements from the sched */
    sched_destroy (sched);
}
