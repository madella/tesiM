#!/bin/bash
BASE="./build"

for transp in "udp" "udpM" ; do
    if [ ! -d "pubs" ]; then
        mkdir "pubs"
    fi
    # Start dummy pub
    taskset -c 1 $BASE/pub outsider $transp 0 &
    secondpub= $!
    wait $secondpub

    # Start real pub
    echo "Dummy done, now starting real"
    for (( jj=1; jj < 5; jj+=2)); do
        if [ $jj -eq 3 ]; then 
            jj=2
        fi
        taskset -c 0 $BASE/pub part* $transp $jj &
        fisrtpub=$!
        wait $fisrtpub
        sleep 5
    done

done
sleep 15
pkill sub
pkill pub
exit 0