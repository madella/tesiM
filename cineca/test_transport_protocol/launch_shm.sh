#!/bin/bash
BASE="/g100/home/userexternal/gmadella/fastdds/build"

sleep=1000000 # 1000000000ns = 1 s
n_of_message=1000
partition="transportTest"

# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
for transport in "shm" ; do
    for (( i=0; i < 47; i++)); do
        taskset -c $i $BASE/sub $transport $sleep $n_of_message $partition $i "data/" &
    done

    taskset -c 47 $BASE/pub $transport $sleep $n_of_message $partition 0 "data/"
done

exit 0

