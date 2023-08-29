#!/bin/bash
BASE="./test_wildcards/build"

if [ ! -d "pubs" ]; then mkdir "pubs" ;fi # Make all directory necessary for store datas

for transp in "udp" ; do
    echo "Starting $transp protocol tests..."
    echo "
    Starting dummy pub"
    taskset -c 1 $BASE/pub outsider $transp 0 &
    secondpub= $!
    echo "waiting for pub..."
    wait $secondpub
    pkill sub

    echo "
    Starting real pub"
    for (( jj=1; jj < 17; jj+=2)); do
        echo "Sleeping $jj to permit subs of creating structures"
        sleep $jj
        if [ $jj -eq 3 ]; then jj=2 ; fi
        taskset -c 0 $BASE/pub "part*" $transp $jj &
        fisrtpub=$!
        echo "waiting for pub..."
        wait $fisrtpub
        echo "deleting remained subs..."
    done

done
echo "waiting 10 second to let everythin close by their own"
sleep 10
pkill sub
pkill pub
echo "movin files"
if [ ! -d "data" ]; then mkdir "data" ;fi # Make all directory necessary for store datas
mv group* data/
mv pubs data/
exit 0
