#!/bin/bash
BASE="/g100/home/userexternal/gmadella/fastdds/build"
#"USAGE: transport sleep_time(nanoseconds) n_of_message partition #sub base/ tcp[ip,port]"
syncB="/g100/home/userexternal/gmadella/sync/build"
rm -rfd data/*
ip=$(hostname -i)
echo $ip
transport=udp
sleep=1000000 
n_of_message=1000
partition="part*"

if [ ! -d "pubs" ]; then mkdir "pubs" ;fi # Make all directory necessary for store datas

for transport in "udp" "udpM" ; do
    sleep 4
    echo "Starting $transport protocol tests..."
    taskset -c 1 $BASE/pub $transport $sleep $n_of_message "outsider" 0 "pubs/" &
    secondpub=$!
    echo "waiting for outsider-pub..."
    wait $secondpub
    
    for (( jj=1; jj < 17; jj+=2)); do
        taskset -c 1 $syncB/pub tcp "TEST2START$jj" $ip 5100; sleep 4

        if [ $jj -eq 3 ]; then jj=2 ; fi
        taskset -c 0 $BASE/pub $transport $sleep $n_of_message "part*" $jj "pubs/" &
        fisrtpub=$!
        echo "waiting for real-pub..."
        wait $fisrtpub
    done

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
