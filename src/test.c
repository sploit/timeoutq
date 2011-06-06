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

int task (const void *data) 
{
    return 0;
};

int compare (const void *p1, const void *p2)
{
    int *val1, *val2;

    val1 = (int *)p1;
    val2 = (int *)p2;

    if (*val1 == *val2)
        return 1;

    return 0;
}

int main (int argc, char *argv[])
{
    struct sched *sched = sched_create ();
    struct sched_element *elem;
    int key;

    /* for nanosleep */
    struct timespec time;
    time.tv_sec = 3;
    time.tv_nsec = 0;

    /* Set the function pointers */
    sched->task = task;
    sched->compare = compare;

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
