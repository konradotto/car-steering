#!/bin/bash
handle_exit() {
	docker kill $(docker ps -q)
	kill -9 %1
}
trap "exit" INT TERM ERR
trap handle_exit EXIT

docker run --rm --net=host --name=decoder --ipc=host -e DISPLAY=$DISPLAY -v /tmp:/tmp h264decoder:v0.0.3 --cid=253 --name=img
