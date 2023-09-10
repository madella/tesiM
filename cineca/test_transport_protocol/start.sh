#!/bin/bash
if [ ! -d "data" ]; then mkdir "data" ;fi # Make all directory necessary for store datas
rm -rfd data/*
BASE="/g100/home/userexternal/gmadella/fastdds/build"
syncB="/g100/home/userexternal/gmadella/sync/build"
ip=$(hostname -i)
echo "Ip is $ip, 10 second befor start..."

sleep=1000000 # 1000000000ns = 1 s
n_of_message=1000
partition="transportTest"

for transport in  "udp" "tcp" "udpM" ; do # "shm" not possible on different nodes
    taskset -c 1 $syncB/pub tcp "syncSTART$transport" $ip 5100

    taskset -c 0 $BASE/pub $transport $sleep $n_of_message "$partition" 0 "data/" $ip 5100
    
    taskset -c 2 $syncB/pub tcp "syncEND$transport" $ip 5100
done

echo "waiting 10 second to let everythin close by their own"
sleep 10
pkill pub
echo "launch shared meme test..."
./launch_shm.sh

exit 0
