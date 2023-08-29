#!/bin/bash
BASE="./test_wildcards/build"

N=$(nproc --all)
echo "Check # of core : $N"
for transp in "udp" ; do
    for (( i=1; i < $N; i++)); do
        taskset -c $N $BASE/sub part$j $transp $i &
        firstsub=$! # Ci sono sicuramente modi più eleganti per prendere l'ultimo PID, ma aihme qui manca il tempo
        echo "waiting for last sub..."
        wait $fisrtpub
    done

done
exit 0

