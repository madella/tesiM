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
    echo "
    
    Entering test with $i node
    
    "
    sudo bash tcpdump_monitor.sh $i $PROTOCOL1 $PARTITIONS

    sudo bash tcpdump_monitor.sh $i $PROTOCOL2 $PARTITIONS

done

python3 discovery_graph.py
exit 0



