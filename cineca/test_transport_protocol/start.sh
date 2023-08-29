#!/bin/bash
BASE="./test_transport/build"
syncB="/g100/home/userexternal/gmadella/sync/build"
ip=$(hostname -i)
echo "Ip is $ip, 10 second befor start..."
# SYNC
$syncB/pub sync tcp 0 $ip 5100
pkill pub
sleep 5
for transp in  "tcp" "udpM" "udp"; do # "shm" not possible on different nodes
  # taskset -c 0 $BASE/pub partit  transp  #ofp ip port &
    sleep 4
    taskset -c 0 $BASE/pub 0 $transp 0 $ip 5100
done

echo "waiting 10 second to let everythin close by their own"
sleep 10
pkill pub

echo "movin files"
if [ ! -d "data" ]; then mkdir "data" ;fi # Make all directory necessary for store datas
mv sub* data/
mv pub* data/
exit 0
