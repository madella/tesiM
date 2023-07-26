#!/bin/bash
export FASTRTPS_DEFAULT_PROFILES_FILE="no_intraprocess_configuration.xml"

usage="usage: $(basename "$0") N_OF_TEST -- analyze messages

positional arguments:
    N_OF_TEST 
"

N_OF_TEST=${1}

if [ $# -lt 1 ]; then
    python3 discovery_packets.py
    exit 1
fi

file1="data/discovery_$N_OF_TEST.data"
file2="data/discovery_server_$N_OF_TEST.data"
rm $file1 $file2 > /dev/null 2>&1 
touch $file1 $file2
perf stat -o $file1 ./perf_monitor.sh ${N_OF_TEST}

perf stat -o $file2 ./perf_monitor.sh ${N_OF_TEST} SERVER

exit 0