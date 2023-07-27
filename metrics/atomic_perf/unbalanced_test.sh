#!/bin/bash

GRANULARITY=20
RANGE=100
BASE=10

PROTOCOL=perf

ps=sub

export FASTRTPS_DEFAULT_PROFILES_FILE="no_intraprocess_configuration.xml"
export PS=$ps

for ((i=0; i<=$RANGE; i+=$GRANULARITY))
do
    #./launch_pubsub $PROTOCOL #SUBon #SUBoff DOMAIN PARTITIONS TOPIC
    ./launch_$ps.sh $PROTOCOL 10 $i 0 0 0
done

python3 multiplecost.py
python3 multiplecost_bar.py
python3 merge.py

exit 0

# declare -A dict
# dict[1]=10
# echo ${dict[1]}