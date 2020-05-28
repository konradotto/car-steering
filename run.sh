#!/bin/bash
#trap "exit" INT TERM ERR

selection=$1


./run-vehicle-view.sh & 
./run-decoder.sh & 
sleep 5
docker run -i --rm --net=host sele $selection
sleep 5 
./run-group13.sh & 
sleep 20

pkill -f ./run-group13.sh
pkill -f ./run-decoder.sh
pkill -f ./run-vehicle-view.sh

mv csv/reports.csv "csv/report${selection}.csv"