#!/bin/bash
BASE="./test_transport_protocol/build"

# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
for transp in "shm" ; do
    for (( i=0; i < 47; i++)); do
        taskset -c $i $BASE/sub $transp $i &
    done

    taskset -c 47 $BASE/pub $transp 0 &
    wait $!

    sleep 30
    pkill sub
done
exit 0

