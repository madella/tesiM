#!/bin/bash
BASE="/g100/home/userexternal/gmadella/fastdds/build"
syncB="/g100/home/userexternal/gmadella/sync/build"
N=$(nproc --all)
if [ $# -lt 1 ]; then
    echo "missing ip"
    exit 0
fi
ip="$1"
sleep=1000000000 # 1000000000ns = 1 s
n_of_message=1000
partition="transportTest"

echo "Check # of core : $N"

for transport in  "udp" "tcp" "udpM" ; do # "shm" not possible on different nodes
    for (( i=0; i < $N; i++)); do
        taskset -c $i $BASE/sub $transport $sleep $n_of_message "$partition" $i "data/" $ip 5100 &  
        lastsub=$!
    done
    taskset -c 0 $syncB/sub tcp "syncSTART$transport" $ip 5100
    echo "waiting for last sub..."
    wait $lastsub
    taskset -c 0 $syncB/sub tcp "syncEND$transport" $ip 5100
done
exit 0

