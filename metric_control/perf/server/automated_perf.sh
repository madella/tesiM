#!/bin/bash

GRANULARITY=${2}
RANGE=${1}

export RANG=$RANGE
export GRAN=$GRANULARITY

for ((i=1; i<=$RANGE; i+=$GRANULARITY))
do
    echo ""
    echo "Entering test with $i node"
    echo ""
    ./start_perf.sh $i
done

#python3 discovery_graph.py
exit 0