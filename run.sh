#!/bin/bash
#trap "exit" INT TERM ERR

./run-vehicle-view.sh & 
./run-decoder.sh & 
sleep 10 
docker run -i --rm --net=host sele 1
sleep 5 
./run-group13.sh & 