#!/bin/bash
BASE="/g100/home/userexternal/gmadella/fastdds/build"
#USAGE: transport sleep_time(nanoseconds) n_of_message partition #sub tcp[ip,port]
syncB="/g100/home/userexternal/gmadella/sync/build"
ip=$1
N=$(nproc --all)

transport=udp
sleep=1000 # 1000000000ns = 1 s
n_of_message=100
partition="part1"

# SYNC
$syncB/sub sync tcp 0 $ip 5100
pkill pub
sleep 5


taskset -c 0 $BASE/sub $transport $sleep $n_of_message $partition 0

wait $!

# Python script for return avg
python3 get_avg_time.py $partition
#mv *.data data/