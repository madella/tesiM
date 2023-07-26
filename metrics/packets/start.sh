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

sudo bash fastdds_generate_discovery_packages.bash ${N_OF_TEST}
echo "
Il risultato dell'operazione è $?
"

sudo bash fastdds_generate_discovery_packages.bash ${N_OF_TEST} SERVER
echo "
Il risultato dell'operazione è $?
"
exit 0