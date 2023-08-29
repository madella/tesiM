#!/bin/bash
BASE="./test_wildcards/build"

if [ ! -d "pubs" ]; then mkdir "pubs" ;fi # Make all directory necessary for store datas

for transp in "udp" ; do
    echo "
    Starting real pub"
    taskset -c 0 $BASE/pub "singlepub" $transp 0 &
    fisrtpub=$!
    echo "waiting for pub..."
    wait $fisrtpub
    echo "deleting remained subs..."
    sleep 1
    pkill sub


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
