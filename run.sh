#!/bin/bash
#trap "exit" INT TERM ERR
trap "exit 0" EXIT

./run-vehicle-view.sh & ./run-decoder.sh & sleep 10 && docker run -i --rm --net=host sele 2 & sleep 5 && ./run-group13.sh & sleep 20 && kill $!
