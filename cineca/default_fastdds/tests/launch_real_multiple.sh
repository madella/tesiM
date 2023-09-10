#!/bin/bash
BASE="/g100/home/userexternal/gmadella/fastdds/build"
syncB="/g100/home/userexternal/gmadella/sync/build"
N=$(nproc --all)
if [ $# -lt 1 ]; then
    ecno "missing ip"
    exit 0
fi
ip=$1
echo "Check # of core : $N"

transport=udp
sleep=1000000000 # 1000000000ns = 1 s
n_of_message=1000
partition="part1"

for cycle in {1..5}; do
    echo "Starting cycke $cycle..."

    if [ ! -d "data$cycle" ]; then
        mkdir "data$cycle"
    fi
    partition="part$cycle"
    for (( i=2; i < $N; i++)); do
        taskset -c $i $BASE/sub $transport "$sleep" "$n_of_message" $partition $i "data$cycle/" >> sub_log.data &  
    done
    taskset -c 1 $BASE/sub $transport "$sleep" "$n_of_message" $partition 1 "data$cycle/" >> sub_log.data &
    last=$!
    taskset -c 0 $syncB/sub tcp syncSTART $ip 5100
    sleep 4
    wait $last
    pkill sub

    taskset -c 0 $syncB/sub tcp syncEND $ip 5100

done

