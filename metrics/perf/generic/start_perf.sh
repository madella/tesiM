#!/bin/bash
export FASTRTPS_DEFAULT_PROFILES_FILE="no_intraprocess_configuration.xml"

usage="usage: $(basename "$0") N_OF_TEST -- analyze messages

positional arguments:
    N_OF_TEST 
"

N_OF_TEST=${1}
PROTOCOL1=${2}
PROTOCOL2=${3}
PARTITIONS=${4}

if [ $# -lt 1 ]; then
    python3 discovery_packets.py
    exit 1
fi

file1="data/${PROTOCOL1}_$N_OF_TEST.data"
file2="data/${PROTOCOL2}_$N_OF_TEST.data"

rm $file1 $file2 > /dev/null 2>&1 
touch $file1 $file2

perf stat -o $file1 ./perf_monitor.sh ${N_OF_TEST} $PROTOCOL1 $PARTITIONS

perf stat -o $file2 ./perf_monitor.sh ${N_OF_TEST} $PROTOCOL2 $PARTITIONS

exit 0