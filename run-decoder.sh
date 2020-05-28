#!/bin/bash
trap "exit" INT TERM ERR
trap "kill 0" EXIT

docker run --rm --net=host --name=decoder --ipc=host -e DISPLAY=$DISPLAY -v /tmp:/tmp h264decoder:v0.0.3 --cid=253 --name=img
