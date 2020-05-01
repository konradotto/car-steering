# Copyright (C) 2020  Christian Berger
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# First stage for building the software:
FROM ubuntu:18.04 as builder
LABEL maintainer="Group13" \
      repo="https://git.chalmers.se/courses/dit638/students/group_13/-/tree/feature/14-hsv-filter"

ENV DEBIAN_FRONTEND noninteractive

# Upgrade the Ubuntu 18.04 LTS base image
RUN apt-get update -y --fix-missing && \
    apt-get upgrade -y && \
    apt-get dist-upgrade -y

# Install the development libraries for OpenCV
RUN apt-get install -y -f --no-install-recommends \
        ca-certificates \
        cmake \
        build-essential \
        libopencv-dev \
        gcovr

# Include this source tree and compile the sources
ADD src/ /opt/sources
WORKDIR /opt/sources
RUN mkdir build && \
    cd build && \
    cmake -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=/tmp .. && \
    make && make install && make test && \
    cd .. && cp templateCone1.png /tmp

# Create a coverage report
#RUN cd /opt/sources && \
#    mkdir coverage && \
#    cd coverage && \
#    g++ -Wall -fprofile-arcs -ftest-coverage -fPIC -O0 ../helloworld/helloworld.cpp ../helloworld/PrimeChecker.cpp -o test-coverage && \
#    ./test-coverage 5 && \
#    gcovr -r . --html --html-details -o test-coverage.html && mkdir /tmp/test && mv *.html /tmp/test && \
#    gcovr -r . --xml-pretty -o test-coverage.xml && mv test-coverage.xml /tmp/test && \
#    gcovr -r . -o summary.txt && mv summary.txt /tmp/test && \
#    rm -fr * && cd .. && rm -d coverage && \
#    cp image-cropping/templateCone1.png /tmp


# Second stage for packaging the software into a software bundle:
FROM ubuntu:18.04
LABEL maintainer="Group13" \
      repo="https://git.chalmers.se/courses/dit638/students/group_13/-/tree/feature/14-hsv-filter"

ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update -y && \
    apt-get upgrade -y && \
    apt-get dist-upgrade -y

RUN apt-get install -y --no-install-recommends \
        libopencv-core3.2 \
        libopencv-highgui3.2 \
        libopencv-imgproc3.2

WORKDIR /opt
COPY --from=builder /tmp/bin/cyberGroup13 .
COPY --from=builder /tmp/ .
# This is the entrypoint when starting the Docker container; hence, this Docker image is automatically starting our software on its creation
ENTRYPOINT ["/opt/cyberGroup13"]