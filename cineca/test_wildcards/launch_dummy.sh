#!/bin/bash
BASE="./build"

N=$(nproc)

# ^^^^^^^^^^^^^^^^DUMMY^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
for transp in "udp" "udpM" ; do
    if [ ! -d "groupd" ]; then
        mkdir "groupd"
    fi
    for (( i=1; i < $N; i++)); do
        taskset -c $i $BASE/sub outsider $transp $i d &
    done
    taskset -c 0 $BASE/sub outsider $transp 0 d &
    fistsub=$!
    wait $fistsub
done
exit 0

