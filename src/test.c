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
#include "timeout.h"

int destroy () { return 0; };

int main (int argc, char *argv[])
{
    struct queue *queue = queue_create ();
    struct queue_element *elem;
    int key;
    struct timespec time;
    time.tv_sec = 3;
    time.tv_nsec = 0;

    /* XXX this is evil, need to check semaphores before setting
     * destructor to avoid potential race conditions.
     */
    queue->destroy = destroy;

    if (queue_start (queue))
        return -1;

    /* Insert 9000 elements into the queue, space every thousand
     * or so 15 seconds apart so that timeout thread will be forced
     * to dump several times */
    for (key = 0; key < 3000; key++)
    {
        if ((key % 1000) == 0)
            nanosleep (&time, NULL);

        elem = queue_element_create (&key, sizeof(key));
        queue_insert (queue, elem);
    }

    /* the timeout thread will ensure that we eventually 
     * remove all the elements from the queue */
    while (queue->size > 0);

    if (queue_stop (queue))
        return -1;

    /* when finished be sure to call destroy as it free's
     * memory and presumably kills the timeout thread. */
    queue_destroy (queue);
}
