#!/bin/bash
handle_exit() {
	docker kill group13
	kill -9 %1
}
trap "exit" INT TERM ERR
trap handle_exit EXIT

xhost +
#docker run --rm --net=host --name=group13 --ipc=host -e DISPLAY=$DISPLAY -v /tmp:/tmp -v $PWD/img:/img -v $PWD/csv:/csv/ group13:latest --cid=253 --name=img --width=640 --height=480 --verbose &
docker run --rm --net=host --name=group13 --ipc=host -v /tmp:/tmp -v $PWD/img:/img -v $PWD/csv:/csv/ group13:latest --cid=253 --name=img --width=640 --height=480 &

wait