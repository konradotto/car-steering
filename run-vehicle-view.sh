#!/bin/bash
handle_exit() {
	docker kill vehicle-view
	kill -9 %1
}
trap "exit" INT TERM ERR
trap handle_exit EXIT

cd ../individual/recordings
docker run --rm --init --net=host --name=vehicle-view -v $PWD:/opt/vehicle-view/recordings -v /var/run/docker.sock:/var/run/docker.sock -p 8081:8081 chalmersrevere/opendlv-vehicle-view-multi:v0.0.60 &
sleep 2
firefox localhost:8081/recordings

wait