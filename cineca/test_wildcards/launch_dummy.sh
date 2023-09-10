#!/bin/bash
BASE="/g100/home/userexternal/gmadella/fastdds/build"
#USAGE: transport sleep_time(nanoseconds) n_of_message partition #sub tcp[ip,port]
syncB="/g100/home/userexternal/gmadella/sync/build"

transport=udp
sleep=10000000 # 1000000000ns = 1 s
n_of_message=1000
partition="part*"

N=$(nproc --all)
if [ ! -d "groupd" ]; then mkdir "groupd"; fi

for transport in "udp" "udpM" ; do
    for (( i=0; i < $N; i++)); do
        taskset -c $i $BASE/sub $transport $sleep $n_of_message outsider $i groupd/ &
        firstsub=$!
    done
    echo "waiting for last dummy-sub..."
    wait $fisrtpub
done
exit 0

