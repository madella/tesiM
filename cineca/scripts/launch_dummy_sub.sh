#!/bin/bash
BASE="./test_wildcars/build"
# ^^^^^^^^^^^^^^^^DUMMY^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
for transp in "udp" "udpM" ; do
    for (( i=1; i < 48; i++)); do
        taskset -c $i $BASE/sub outsider $transp $i &
    done
    taskset -c 0 $BASE/sub outsider $transp 0 &
    fistsub=$!
    wait $fistsub
    sleep 1
    pkill sub
done
exit 0

