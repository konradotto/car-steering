#!/bin/bash
trap "exit" INT TERM ERR
trap "kill SIGINT" EXIT

./run-vehicle-view.sh &
./run-decoder.sh &
sleep 3
docker run -i --rm --net=host sele 2
sleep 3
./run-group13.sh &

sleep 20

kill SIGINT