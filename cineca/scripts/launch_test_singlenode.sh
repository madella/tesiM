#!/bin/bash
BASE="./test_wildcars/build"

for transp in "udp" "udpM" ; do
# ^^^^^^^^^^^^^^^^DUMMY^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    # for (( i=0; i < 47; i++)); do
    #     taskset -c $i $BASE/sub outsider $transp $i 
    # done

    # taskset -c 47 $BASE/pub outsider $transp 1 
    # wait $!

    # pkill sub
    # sleep 1 
    #the dummy needs to started on another node
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    for (( jj=1; jj < 13; jj+=2)); do
        if [ $jj -eq 3 ]; then 
            jj=2
        fi
        for (( j=1; j <= $jj ; j++)); do
            for (( i=0; i < $(( 48 / $jj)); i++)); do
                taskset -c $(( ( j - 1 ) * ( 48 / jj ) + i )) $BASE/sub part$j $transp $i 
            done
        done
        taskset -c 47 $BASE/pub part* $transp 0 
        wait $!

        pkill sub
        sleep 1
    done
done
exit 0

