#!/bin/bash
#trap "exit" INT TERM ERR

set -e

./run-vehicle-view.sh & 
./run-decoder.sh & 
sleep 10 
docker run -i --rm --net=host sele 1
sleep 5 
./run-group13.sh & 
sleep 20

pkill -f ./run-group13.sh
pkill -f ./run-decoder.sh
pkill -f ./run-vehicle-view.sh