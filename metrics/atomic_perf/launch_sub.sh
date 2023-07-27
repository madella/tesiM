#!/bin/bash

PROTOCOL=${1}
SUBon=${2}
SUBoff=${3}
D=${4}
P=${5} 
T=${6}


echo "Starting $0 with PRTCL:$PROTOCOL, #Son $SUBon, #Soff $SUBoff"
BASE="/media/dati/tesi/tesiM/c++/dds_${PROTOCOL}/build"

# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
DUMP_FILE="data/cht_${SUBon}_${SUBoff}.data"
echo $DUMP_FILE
for (( i=0; i < $SUBoff; i++)); do
    $BASE/pub $D $P off > /dev/null &
done
for (( i=0; i < $SUBon; i++)); do
    $BASE/pub $D $P $T > /dev/null &
done

$BASE/sub $D $P $T > $DUMP_FILE &
wait $!

pkill pub
sleep 5
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
DUMP_FILE="data/chp_${SUBon}_${SUBoff}.data"
echo $DUMP_FILE
for (( i=0; i < $SUBoff; i++)); do
    $BASE/pub $D off $T > /dev/null &
done
for (( i=0; i < $SUBon; i++)); do
    $BASE/pub $D $P $T > /dev/null &
done

$BASE/sub $D $P $T > $DUMP_FILE &
wait $!
pkill pub
sleep 5

# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
DUMP_FILE="data/chd_${SUBon}_${SUBoff}.data"
echo $DUMP_FILE
for (( i=0; i < $SUBoff; i++)); do
    $BASE/pub 10 $P $T > /dev/null &
done
for (( i=0; i < $SUBon; i++)); do
    $BASE/pub $D $P $T > /dev/null &
done

$BASE/sub $D $P $T > $DUMP_FILE &
wait $!
pkill pub
sleep 5

# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
SUBon=$SUBoff
DUMP_FILE="data/std_${SUBon}_0.data"
echo $DUMP_FILE
if [ $SUBon -eq 0 ]; then 
    $BASE/pub $D $P $T > /dev/null &
fi
for (( i=0; i < $SUBon; i++)); do
    $BASE/pub $D $P $T > /dev/null &
done
$BASE/sub $D $P $T > $DUMP_FILE &
wait $!
pkill pub

exit 0