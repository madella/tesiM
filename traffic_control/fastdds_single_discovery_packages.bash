#!/bin/bash
usage="usage: $(basename "$0") BASE [PROTOCOL] [-h] -- analyze network trafic of ros2 nodes discovery messages

positional arguments:
    BASE 
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
BASE=50

# If second argument is SERVER it uses Discovery Service
PROTOCOL=${2}
ADD=${1}

# Dump file for capture
DUMP_FILE="pcapng/simple_$ADD.pcapng"
if [[ ${PROTOCOL} == "SERVER" ]]
then
    DUMP_FILE="pcapng/discovery_server_$ADD.pcapng"
    echo "Run in Discovery Server mode"
else
    unset ROS_DISCOVERY_SERVER
    echo "Run in Simple Discovery mode"
fi

# Time running
RUN_TIME=$(($BASE * 3 / 10))
echo "RUNTIME: $RUN_TIME with $BASE"


# Start talker in SERVER or SIMPLE mode
BASE_NO_SERV="/media/dati/tesi/tesiM/c++/dds_server/build"
BAE_SERV="/media/dati/tesi/tesiM/c++/dds_helloWorld/build"
if [[ ${PROTOCOL} == "SERVER" ]]
then
    $BASE_NO_SERV/serv &
    DIS_PID=$!
    sleep 3
    echo "Spawn talker"
    $BASE_NO_SERV/pub &
    for ((i=1; i<=$BASE; i++))
    do
        $BASE_NO_SERV/sub &
    done
else
    echo "Spawn talker"
    $BAE_SERV/pub &

    for ((i=1; i<=$BASE; i++))
    do
        $BAE_SERV/sub &    
    done

fi

# Start capture
rm -f ${DUMP_FILE} > /dev/null 2>&1
sudo tcpdump -G $((RUN_TIME + 2)) -W 1 -i any -nn -s 0 -w ${DUMP_FILE} > /dev/null 2>&1 &
TCPDUMP_PID=$!


for ((i=1; i<=$ADD; i++))
do
    echo "Spawn ADD"
    $BAE_SERV/sub &
done

# Wait for tcpdump to finish and send ctrl-c to talker and listeners
sleep $RUN_TIME
kill -s SIGINT $(ps -C pub -o pid=) 2>&1
kill -s SIGINT $(ps -C sub -o pid=) 2>&1

# Ends all discovery servers
if [[ ${PROTOCOL} == "SERVER" ]]
then
    kill -s SIGINT $DIS_PID 2>&1
fi

sleep 1

echo "Traffic capture can be found in: ${DUMP_FILE}"

# Make sure they are killed
pkill pub
pkill sub
pkill serv