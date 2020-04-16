##################################################
# Section 1: Build the application
FROM ubuntu:18.04 as builder
MAINTAINER Jean Paul Massoud gusjeanma@student.gu.se

RUN apt-get update -y && \
    apt-get upgrade -y && \
    apt-get dist-upgrade -y

RUN apt-get install -y --no-install-recommends \
        cmake \
        build-essential \
        gcovr

ADD src/ /opt/sources
WORKDIR /opt/sources

RUN cd /opt/sources && \
    mkdir build && \
    cd build && \
    cmake -D CMAKE_BUILD_TYPE=Release .. && \
    make && make test && cp helloworld /tmp

RUN cd /opt/sources && \
    mkdir coverage && \
    cd coverage && \
    g++ -Wall -fprofile-arcs -ftest-coverage -fPIC -O0 ../helloworld.cpp ../PrimeChecker.cpp -o test-coverage && \
    ./test-coverage 5 && \
    gcovr -r . && \
    gcovr -r . --html --html-details -o test-coverage.html && mkdir /tmp/test && mv *.html /tmp/test && \
    rm -fr * && cd .. && rm -d coverage



##################################################
# Section 2: Bundle the application.
FROM ubuntu:18.04
MAINTAINER Jean Paul Massoud gusjeanma@student.gu.se

RUN apt-get update -y && \
    apt-get upgrade -y && \
    apt-get dist-upgrade -y


WORKDIR /opt
COPY --from=builder /tmp/ .

ENTRYPOINT ["/opt/helloworld"]