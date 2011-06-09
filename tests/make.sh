#!/bin/bash

gcc -o check_tmq -pthread -lcheck -ldl -I../src ../src/timeout_queue.c check_tmq.c

# tmq_element_create tests
gcc -o check_elem_create -pthread -lcheck -I../src ../src/timeout_queue.c check_elem_create.c

# tmq_insert tests
gcc -o check_insert -pthread -lcheck -I../src ../src/timeout_queue.c check_insert.c

# tmq_delete tests
gcc -o check_delete -pthread -lcheck -I../src ../src/timeout_queue.c check_delete.c
