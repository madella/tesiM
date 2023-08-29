#!/bin/bash
BASE="./test_transport/build"
syncB="/g100/home/userexternal/gmadella/sync/build"
N=$(nproc --all)
if [ $# -lt 1 ]; then
    print "missing ip"
    exit 0
fi
ip=$1

# SYNC
$syncB/sub sync tcp 0 $ip 5100
pkill sub
sleep 5
echo "Check # of core : $N"
for transp in  "tcp" "udpM" "udp"; do # "shm" not possible on different nodes
    for (( i=1; i < $N; i++)); do
        taskset -c $i $BASE/sub 0 $transp $i $ip 5100 &  
    done
    taskset -c 0 $BASE/sub 0 $transp 0 $ip 5100 &
    lastsub=$!
    echo "waiting for last sub..."
    wait $lastsub
done
exit 0

