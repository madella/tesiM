#!/bin/bash

GRANULARITY=10
RANGE=101
PARTITIONS=1

PROTOCOL1=customtopic
PROTOCOL2=multicast


export FASTRTPS_DEFAULT_PROFILES_FILE="no_intraprocess_configuration.xml"
export RANG=$RANGE
export GRAN=$GRANULARITY
export PROT1=$PROTOCOL1
export PROT2=$PROTOCOL2


for ((i=1; i<=$RANGE; i+=$GRANULARITY))
do
echo ""
echo "Entering test with $i node"
echo ""

    file1="data/${PROTOCOL1}_$i.data"
    file2="data/${PROTOCOL2}_$i.data"

    rm $file1 $file2 > /dev/null 2>&1 
    touch $file1 $file2

    perf stat -o $file1 ./perf_monitor.sh ${i} $PROTOCOL1 $PARTITIONS

    perf stat -o $file2 ./perf_monitor.sh ${i} $PROTOCOL2 $PARTITIONS

done

python3 graph_generator.py
exit 0
