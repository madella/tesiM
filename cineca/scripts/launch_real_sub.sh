#!/bin/bash
BASE="./test_wildcars/build"

for transp in "udp" "udpM" ; do

# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    for (( jj=1; jj < 13; jj+=2)); do
        if [ $jj -eq 3 ]; then 
            jj=2
        fi
        for (( j=1; j <= $jj ; j++)); do
            for (( i=1; i < $(( 48 / $jj)); i++)); do
                taskset -c $(( ( j - 1 ) * ( 48 / jj ) + i )) $BASE/sub part$j $transp $i &
            done
        done
        taskset -c 0 $BASE/sub part$j $transp 0 &
        firstsub=$!

    wait $fisrtpub
    sleep 1

    done
done
exit 0

