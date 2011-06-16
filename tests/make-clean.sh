#!/bin/bash
find . -name "check_tmq_*" -not -name "make.sh" -not -name "*\.c" -exec rm {} \;
