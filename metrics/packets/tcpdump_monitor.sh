#!/bin/bash
usage="usage: $(basename "$0") N_OF_TEST PROTOCOL #PARTITIONS [-h] -- analyze perfs of different approaches on FastDDS
positional arguments:
    N_OF_TEST 
options:
    -h  show this help text"
seed=42
while getopts ':h:' option; do
  case "$option" in
    h) echo "$usage"
       exit
       ;;
   \?) printf "illegal option: -%s\n" "$OPTARG" >&2
       echo "$usage" >&2
       exit 1
       ;;
  esac
done
shift $((OPTIND - 1))

if [ $# -lt 2 ]; then
    if [[ $1 =~ ^[0-9]+$ ]]; then
        echo "Starting with $1 nodes "
    else
        echo "$1 is not a number."
        echo $usage
        exit 1
    fi
fi

N_OF_TEST=${1}
PROTOCOL=${2}
PARTITIONS=${3}

DUMP_FILE="data/${PROTOCOL}_$N_OF_TEST.data"
# Start capture
rm -f ${DUMP_FILE} > /dev/null 2>&1
sudo tcpdump -W 1 -G 5 -i lo -nn -s 0 -w ${DUMP_FILE} > /dev/null 2>&1 &
TCPDUMP_PID=$!

N=$(( $N_OF_TEST / $PARTITIONS ))
if [ $N -eq 0 ]; then
    N=1
fi

PROTB1="/media/dati/tesi/tesiM/c++/dds_${PROTOCOL}/build"

for (( j=0; j < $PARTITIONS; j++)); do
    var_name="PUBPID$j"
    for (( i=0; i < N; i++)); do
        $PROTB1/sub $j &
        sleep 0.1
    done
     sleep 0.1
    $PROTB1/pub $j &
    save=$!
    eval "$var_name=$save"
done

for (( j=0; j < $PARTITIONS; j++)); do
    var_name="PUBPID$j"
    echo "waiting for pub as pid ${!var_name}"
    wait ${!var_name}
    
done

echo "waiting for tcpdump as pid ${!var_name}"
wait $TCPDUMP_PID
pkill pub
pkill sub
sleep 0.1
exit 0