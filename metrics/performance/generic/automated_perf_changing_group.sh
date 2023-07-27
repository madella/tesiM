#!/bin/bash

GRANULARITY=10
RANGE=60

NODE=60

PROTOCOL1=customtopic
PROTOCOL2=partitions


export FASTRTPS_DEFAULT_PROFILES_FILE="no_intraprocess_configuration.xml"
export RANG=$RANGE
export GRAN=$GRANULARITY
export PROT1=$PROTOCOL1
export PROT2=$PROTOCOL2


for ((i=1; i<=$RANGE; i+=$GRANULARITY))
do
echo ""
echo "Entering test with $i partitions"
echo ""

    file1="data/${PROTOCOL1}_partition_$i.data"
    file2="data/${PROTOCOL2}_partition_$i.data"

    rm $file1 $file2 > /dev/null 2>&1 
    touch $file1 $file2

    perf stat -o $file1 ./perf_monitor.sh 60 $PROTOCOL1 $i

    perf stat -o $file2 ./perf_monitor.sh 60 $PROTOCOL2 $i

done

python3 graph_generator_partition.py
exit 0
