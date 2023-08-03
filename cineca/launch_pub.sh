#!/bin/bash
BASE="./test_transport_protocol/build"

# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
for transp in "tcp" "udp" "udpM" "shm" ; do
    for (( i=0; i < 7; i++)); do
        taskset -c $i $BASE/sub $transp $i &
    done

    taskset -c 7 $BASE/pub $transp 1 &
    wait $!

    pkill sub
    sleep 3
done
exit 0

