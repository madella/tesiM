#!/bin/bash
BASE="./test_wildcards/build"

N=$(nproc --all)
echo "Check # of core : $N"
for transp in "udp" "udpM" ; do
    ## JJ = Numero di partizioni massimo == 12, cicla da jj=1, a jj=12 con uno step di 2
    for (( jj=1; jj < 13; jj+=2)); do
        echo "creating sub divided in $jj partitions"
        if [ $jj -eq 3 ]; then jj=2 ; fi # Modo poco elegante di fare 1,2,4,6,8,...
        if [ ! -d "group$jj" ]; then mkdir "group$jj" ; fi
        # Creazione dei vari sub nelle diverse partitions, ricordo jj, ci dice quante ce ne sono, e in automatico li divido nei core disponibili
        for (( j=1; j <= $jj ; j++)); do
            for (( i=0; i < $(( $N / $jj)); i++)); do
                taskset -c $(( ( j - 1 ) * ( $N / jj ) + i )) $BASE/sub part$j $transp $i $jj &
                firstsub=$! # Ci sono sicuramente modi più eleganti per prendere l'ultimo PID, ma aihme qui manca il tempo
            done
        done
        # Aspetto che l'ultimo abbia finito
        echo "waiting for last sub..."
        wait $fisrtpub
    done
done
exit 0

