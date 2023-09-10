#!/bin/bash
BASE="/g100/home/userexternal/gmadella/fastdds/build"
syncB="/g100/home/userexternal/gmadella/sync/build"
ip=$(hostname -i)
echo "Ip is $ip"
transport=udp
sleep=1 # 1000000000ns = 1 s
n_of_message=1000
partition="part1"


$syncB/pub sync tcp 0 $ip 5100 > pub_log.data
pkill pub
sleep 5
taskset -c 0 $BASE/pub "$transport" $sleep $n_of_message "$partition" 0 >> pub_log.data

python3 get_avg_time.py $partition
rm *.data

