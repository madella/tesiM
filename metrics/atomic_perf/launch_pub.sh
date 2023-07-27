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
    $BASE/sub $D $P off &
done
for (( i=0; i < $SUBon; i++)); do
    $BASE/sub $D $P $T &
done

$BASE/pub $D $P $T > $DUMP_FILE &
wait $!

pkill sub
sleep 5
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
DUMP_FILE="data/chp_${SUBon}_${SUBoff}.data"
echo $DUMP_FILE
for (( i=0; i < $SUBoff; i++)); do
    $BASE/sub $D off $T &
done
for (( i=0; i < $SUBon; i++)); do
    $BASE/sub $D $P $T &
done

$BASE/pub $D $P $T > $DUMP_FILE &
wait $!
pkill sub
sleep 5

# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
DUMP_FILE="data/chd_${SUBon}_${SUBoff}.data"
echo $DUMP_FILE
for (( i=0; i < $SUBoff; i++)); do
    $BASE/sub 10 $P $T &
done
for (( i=0; i < $SUBon; i++)); do
    $BASE/sub $D $P $T &
done

$BASE/pub $D $P $T > $DUMP_FILE &
wait $!
pkill sub
sleep 5

# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
SUBon=$SUBoff
DUMP_FILE="data/std_${SUBon}_0.data"
echo $DUMP_FILE
if [ $SUBon -eq 0 ]; then 
    $BASE/sub $D $P $T &
fi
for (( i=0; i < $SUBon; i++)); do
    $BASE/sub $D $P $T &
done
$BASE/pub $D $P $T > $DUMP_FILE &
wait $!
pkill sub

exit 0