#!/bin/bash
trap "exit" INT TERM ERR
trap "docker kill $(docker ps -q) && kill 0" EXIT

./run-vehicle-view.sh &
./run-decoder.sh &
sleep 10
./run-group13.sh &

wait
