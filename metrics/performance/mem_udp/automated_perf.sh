#!/bin/bash

GRANULARITY=10
PARTITIONS=3
BASE=1 # default 1
RANGE=101

PROTOCOL1=customtopic
PROTOCOL2=sharedmemory

export FASTRTPS_DEFAULT_PROFILES_FILE="no_intraprocess_configuration.xml"
export RANG=$RANGE
export GRAN=$GRANULARITY
export PROT1=$PROTOCOL1
export PROT2=$PROTOCOL2
if [ $#  -gt 0 ]; then
    python3 graph_generator.py
else
   # In this case i have to perform 1 single test, with base > 50
    for ((i=$BASE; i<=$BASE+$RANGE; i+=$GRANULARITY))
    do
        echo ""
        echo "Entering test with $i node"

        file1="data/${PROTOCOL1}_$i.data"
        file2="data/${PROTOCOL2}_$i.data"

        rm $file1 $file2 > /dev/null 2>&1 
        touch $file1 $file2

        perf stat -e task-clock,cycles,instructions,cache-references,cache-misses,mem-loads,mem-stores -o $file1 ./perf_monitor.sh ${i} $PROTOCOL1 $PARTITIONS

        perf stat -e task-clock,cycles,instructions,cache-references,cache-misses,mem-loads,mem-stores -o $file2 ./perf_monitor.sh ${i} $PROTOCOL2 $PARTITIONS

    done
    python3 graph_generator.py
fi
exit 0
