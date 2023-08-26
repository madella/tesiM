#!/bin/bash
BASE="./build"

N=$(nproc --all)
if [ ! -d "groupd" ]; then mkdir "groupd"; fi

for transp in "udp" "udpM" ; do
    for (( i=1; i < $N; i++)); do
        taskset -c $i $BASE/sub outsider $transp $i d &
    done
    taskset -c 0 $BASE/sub outsider $transp 0 d &
    fistsub=$!
    echo "waiting for last sub"
    wait $fistsub
done
exit 0

