#!/bin/bash

# tmq_create
gcc -o check_tmq_create -pthread -lcheck -ldl -I../src ../src/timeout_queue.c check_tmq_create.c

# tmq_destroy
gcc -o check_tmq_destroy -pthread -lcheck -ldl -I../src ../src/timeout_queue.c check_tmq_destroy.c

# tmq_element_create tests
gcc -o check_elem_create -pthread -lcheck -I../src ../src/timeout_queue.c check_elem_create.c

# tmq_insert tests
gcc -o check_insert -pthread -lcheck -I../src ../src/timeout_queue.c check_insert.c

# tmq_delete tests
gcc -o check_delete -pthread -lcheck -I../src ../src/timeout_queue.c check_delete.c