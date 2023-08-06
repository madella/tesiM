#!/bin/bash
BASE="./build"

N=$(nproc)

for transp in "udp" "udpM" ; do
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    for (( jj=1; jj < 5; jj+=2)); do
        if [ $jj -eq 3 ]; then 
            jj=2
        fi
        if [ ! -d "group$jj" ]; then
            mkdir "group$jj"
        fi
        for (( j=1; j <= $jj ; j++)); do
            for (( i=1; i < $(( $N / $jj)); i++)); do
                taskset -c $(( ( j - 1 ) * ( $N / jj ) + i )) $BASE/sub part$j $transp $i $jj &
            done
        done
        taskset -c 0 $BASE/sub part$j $transp 0 $jj &
        firstsub=$!
        wait $fisrtpub
    done
done
exit 0

