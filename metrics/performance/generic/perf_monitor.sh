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
N=$(( $N_OF_TEST / $PARTITIONS ))
if [ $N -eq 0 ]; then
    N=1
fi

echo "starting test with $N entities, protocol: $PROTOCOL ,partitions:$PARTITIONS, in $DUMP_FILE"


PROTB1="/media/dati/tesi/tesiM/c++/dds_${PROTOCOL}/build"
for (( j=0; j < $PARTITIONS; j++)); do
    var_name="PUBPID$j"
    for (( i=0; i < $N; i++)); do
        $PROTB1/sub $j &
    done
    $PROTB1/pub $j &
    save=$!
    eval "$var_name=$save"
done
for (( j=0; j < $PARTITIONS; j++)); do
    var_name="PUBPID$j"
    wait ${!var_name}
    echo ${!var_name}

done
pkill pub
pkill sub
exit 0