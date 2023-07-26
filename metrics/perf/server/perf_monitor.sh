#!/bin/bash
usage="usage: $(basename "$0") N_OF_TEST [PROTOCOL] [-h] -- analyze perfs of different approaches on FastDDS
positional arguments:
    N_OF_TEST 
    [optional] PROTOCOL if is SERVER it uses Discovery Service else it uses Simple Discovery
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
else
    if [[ $2 == "SERVER" ]]; then
        echo SERVER mode
    fi
fi

N_OF_TEST=${1}
PROTOCOL=${2}

DUMP_FILE="perf/simple_$N_OF_TEST.perf"
if [[ ${PROTOCOL} == "SERVER" ]]; then
    DUMP_FILE="perf/discovery_server_$N_OF_TEST.perf"
fi

start_perf_capture() {
    perf stat -e cpu-clock,task-clock,context-switches,cpu-migrations,cycles,instructions,duration_time -o $DUMP_FILE -- &
    PERF_PID=$!
}
stop_perf_capture() {
    kill -s SIGINT $PERF_PID
}

SERV="/media/dati/tesi/tesiM/c++/dds_server/build"
SIMPLE="/media/dati/tesi/tesiM/c++/dds_helloWorld/build"
if [[ ${PROTOCOL} == "SERVER" ]]; then
    $SERV/serv &
    DIS_PID=$!
    sleep 1
    $SERV/sub &
    SUBPID=$!
    for (( i=1; i < $N_OF_TEST; i++)); do
        $SERV/sub &
    done
    $SERV/pub &
    PUBPID=$!
else
    sleep 1 # to obtain same sleep of server
    $SIMPLE/sub &
    SUBPID=$!
    for ((i=1; i <= $N_OF_TEST; i++)); do
        $SIMPLE/sub &
    done
    $SIMPLE/pub &
    PUBPID=$!
fi
wait $PUBPID

# Ends all discovery servers
if [[ ${PROTOCOL} == "SERVER" ]]; then
    kill -9 $DIS_PID 
fi

wait $SUBPID 

pkill pub
pkill sub
pkill serv

exit 0