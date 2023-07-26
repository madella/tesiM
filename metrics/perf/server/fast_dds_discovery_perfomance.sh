#!/bin/bash
usage="usage: $(basename "$0") N_OF_TEST [PROTOCOL] [-h] -- analyze cpu usage of FastDDS discovery protocols
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

# First argument must be setup.bash of ROS2
N_OF_TEST=${1}

# If the second argument is SERVER, it uses Discovery Service
PROTOCOL=${2}

# Function to capture process metrics with pidstat
capture_metrics() {
  local PID=$1
  local OUTPUT_FILE=$2
  local time=$3
  pidstat -p $PID -h -r -u -t 1 $time > "$OUTPUT_FILE" &
  PIDSTAT_PID=$!
}

# Dump file for capture
OUTPUT="metrics/simple_discovery_$N_OF_TEST.txt"
if [[ ${PROTOCOL} == "SERVER" ]]; then
    OUTPUT="pcapng/discovery_server_$N_OF_TEST.pcapng"
    echo "Run in Discovery Server mode"
else
    unset ROS_DISCOVERY_SERVER
    echo "Run in Simple Discovery mode"
fi

# Time running
RUN_TIME=$(($N_OF_TEST * 4 / 10))
echo "RUNTIME: $RUN_TIME with $N_OF_TEST"

# Capture metrics for the processes
capture_metrics $$ $OUTPUT $RUN_TIME

# Start talker in

BASE_NO_SERV="/media/dati/tesi/tesiM/c++/dds_server/build"
BASE_SERV="/media/dati/tesi/tesiM/c++/dds_helloWorld/build"
if [[ ${PROTOCOL} == "SERVER" ]]; then
    $BASE_NO_SERV/serv &
    DIS_PID=$!
    sleep 3
    echo "Spawn talker"
    $BASE_NO_SERV/pub &
    echo "Spawn first listener 0"
    $BASE_NO_SERV/sub &
    for ((i=1; i<=$N_OF_TEST; i++))
    do
        perf stat -e cycles $BASE_NO_SERV/sub & > OUTPUT
    done
else
    echo "Spawn talker"
    $BASE_SERV/pub &
    echo "Spawn first listener 0"
    $BASE_SERV/sub &
    for ((i=1; i<=$N_OF_TEST; i++))
    do
        perf stat -e cycles $BASE_SERV/sub & > OUTPUT
    done
fi

# Wait for tcpdump to finish and send ctrl-c to talker and listeners
sleep $RUN_TIME
kill -s SIGINT $(ps -C pub -o pid=) 2>&1
kill -s SIGINT $(ps -C sub -o pid=) 2>&1

# Ends all discovery servers
if [[ ${PROTOCOL} == "SERVER" ]]; then
    kill -s SIGINT $DIS_PID 2>&1
fi

sleep 1

echo "Traffic capture can be found in: ${OUTPUT}"

# Make sure they are killed
pkill pub
pkill sub
pkill serv