#!/bin/bash
BASE="/g100/home/userexternal/gmadella/fastdds/build"
#"USAGE: transport sleep_time(nanoseconds) n_of_message partition #sub base/ tcp[ip,port]"
syncB="/g100/home/userexternal/gmadella/sync/build"

if [ $# -lt 1 ]; then
    ecno "missing ip"
    exit 1
else 
    ip="$1"
fi

transport=udp
sleep=10000000 # 1000000000ns = 1 s
n_of_message=1000
partition="part*"

N=$(nproc --all)
echo "Check # of core : $N"
for transport in "udp" "udpM" ; do
    ## JJ = Numero di partizioni massimo == 12, cicla da jj=1, a jj=12 con uno step di 2
    for (( jj=1; jj < 17; jj+=2)); do
        taskset -c 1 $syncB/sub tcp "TEST2START$jj" $ip 5100;sleep 4

        echo "creating sub divided in $jj partitions"
        if [ $jj -eq 3 ]; then jj=2 ; fi # Modo poco elegante di fare 1,2,4,6,8,...
        if [ ! -d "group$jj" ]; then mkdir "group$jj" ; fi
        # Creazione dei vari sub nelle diverse partitions, ricordo jj, ci dice quante ce ne sono, e in automatico li divido nei core disponibili
        for (( j=1; j <= $jj ; j++)); do
            for (( i=0; i < $(( $N / $jj)); i++)); do
                taskset -c $(( ( j - 1 ) * ( $N / jj ) + i )) $BASE/sub $transport $sleep $n_of_message "part$j" $i "group$jj/"  &
                firstsub=$! # Ci sono sicuramente modi più eleganti per prendere l'ultimo PID, ma aihme qui manca il tempo
            done
        done
        echo "waiting for last real-sub..."
        wait $fisrtpub
    done
done
exit 0

