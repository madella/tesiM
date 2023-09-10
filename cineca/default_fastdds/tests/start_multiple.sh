#!/bin/bash
BASE="/g100/home/userexternal/gmadella/fastdds/build"
syncB="/g100/home/userexternal/gmadella/sync/build"
ip=$(hostname -i)
echo "Ip is $ip"
transport=udp
sleep=1000000 # 1000000000ns = 1 s
n_of_message=1000
partition="part*"
rm -rfd data*/

for cycle in {1..5}; do
    echo "Starting cycke $cycle..."
    if [ ! -d "data$cycle" ]; then
        mkdir "data$cycle"
    fi
    taskset -c 1 $syncB/pub tcp syncSTART $ip 5100

    taskset -c 0 $BASE/pub "$transport" $sleep $n_of_message "$partition" 0 "data$cycle/" >> pub_log.data
    taskset -c 1 $syncB/pub tcp syncEND $ip 5100
    wait $!
done 


for cycle in {1..5}; do
    python3 get_avg_time.py "data$cycle"
done