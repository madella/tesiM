#!/bin/bash

GRANULARITY=10
RANGE=100

PROTOCOL1=customtopicname
PROTOCOL2=partitions

export RANG=$RANGE
export GRAN=$GRANULARITY
export PROT1=$PROTOCOL1
export PROT2=$PROTOCOL2

for ((i=1; i<=$RANGE; i+=$GRANULARITY))
do
    echo ""
    echo "Entering test with $i node"
    echo ""
    ./start_perf.sh $i $PROTOCOL1 $PROTOCOL2
done

python3 discovery_perf.py
exit 0