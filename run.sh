#!/bin/bash
trap "exit" INT TERM ERR
trap "kill 0" EXIT

./run-vehicle-view.sh &
sleep 5
./run-decoder.sh &

wait
