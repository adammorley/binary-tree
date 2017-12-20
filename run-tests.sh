#!/bin/bash

make clean
make all
./test && \
    ./many-test && \
    echo 'ok'
