#!/bin/bash

set -euo pipefail

rm -f send.dat receive.dat sender-packets.log receiver-packets.log
dd if=/dev/urandom of=send.dat bs=5000 count=1

LD_PRELOAD="./log-packets.so" \
    PACKET_LOG="receiver-packets.log" \
    DROP_PATTERN="1010" \
    ./file-receiver receive.dat 1234 4 &
RECEIVER_PID=$!
sleep .1

LD_PRELOAD="./log-packets.so" \
    PACKET_LOG="sender-packets.log" \
    DROP_PATTERN="01010" \
    ./file-sender send.dat localhost 1234 2

wait $RECEIVER_PID || true

diff -qs send.dat receive.dat

./generate-msc.sh msc.eps sender-packets.log receiver-packets.log

rm -f send.dat receive.dat sender-packets.log receiver-packets.log
