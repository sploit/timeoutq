#!/bin/bash

PATH=$PATH:.

# tmq_create
gcc -o check_tmq_create -pthread -lcheck -ldl -I../src ../src/timeout_queue.c check_tmq_create.c

# tmq_destroy
gcc -o check_tmq_destroy -pthread -lcheck -ldl -I../src ../src/timeout_queue.c check_tmq_destroy.c

# tmq_element_create tests
gcc -o check_tmq_element_create -pthread -lcheck -I../src ../src/timeout_queue.c check_tmq_element_create.c

# tmq_insert tests
gcc -o check_tmq_insert -pthread -lcheck -I../src ../src/timeout_queue.c check_tmq_insert.c

# tmq_find tests
gcc -o check_tmq_find -pthread -lcheck -I../src ../src/timeout_queue.c check_tmq_find.c

# tmq_bump tests
gcc -o check_tmq_bump -pthread -lcheck -I../src ../src/timeout_queue.c check_tmq_bump.c

# tmq_pop
gcc -o check_tmq_pop -pthread -lcheck -I../src ../src/timeout_queue.c check_tmq_pop.c

# tmq_delete tests
gcc -o check_tmq_delete -pthread -lcheck -I../src ../src/timeout_queue.c check_tmq_delete.c

# tmq_timeout tests
gcc -o check_tmq_timeout -pthread -lcheck -I../src ../src/timeout_queue.c check_tmq_timeout.c

# tmq_start tests
gcc -o check_tmq_start -pthread -lcheck -I../src ../src/timeout_queue.c check_tmq_start.c

# tmq_stop tests
gcc -o check_tmq_stop -pthread -lcheck -I../src ../src/timeout_queue.c check_tmq_stop.c

# Test for the multithreaded portion
gcc -o timeout_test -pthread -I../src ../src/timeout_queue.c timeout_test.c

# Run the tests
make-check.sh
