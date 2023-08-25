#!/bin/bash
BASE="./test_wildcars/build"

for transp in "udp" "udpM" ; do

    # Start dummy pub
    taskset -c 1 $BASE/pub outsider $transp 0 &
    secondpub= $!
    wait $secondpub
    pkill sub

    # Start real pub
    echo "Dummy done, now starting real"
    for (( jj=1; jj < 13; jj+=2)); do
        if [ $jj -eq 3 ]; then 
            jj=2
        fi
        taskset -c 0 $BASE/pub part* $transp $jj &
        fisrtpub=$!
        wait $fisrtpub
    done

done